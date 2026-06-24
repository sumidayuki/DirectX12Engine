#include "UICanvasSystem.h"
#include "ShaderRegistry.h"

struct UICanvasSystem::ConstantBufferLayout
{
    struct Ortho
    {
        Matrix4x4 orthoMatrix;
    };

    struct Object
    {
        Matrix4x4 world;
        Color color;
        Vector2 minUV;
        Vector2 maxUV;
        UINT textureIndex;
        UINT hasTexture;
        UINT padding[2];
    };
};

void UICanvasSystem::Start(World& world)
{
    const UINT slotSize = (sizeof(ConstantBufferLayout::Object) + 255) & ~255;
    const UINT totalSlots = (1 + MAX_UI_PER_FRAME) * Graphics::BackBafferCount;
    m_constantBufferRing.Attach(new GraphicsBuffer(
        GraphicsBuffer::Target::Constant,
        GraphicsBuffer::UsageFlags::LockBufferForWrite,
        totalSlots,
        slotSize
    ));
    m_mappedConstants = (BYTE*)m_constantBufferRing->LockBufferForWrite();

    struct QV { Vector2 p; Vector2 uv; };
    QV verts[4] = {
        { {0,0}, {0,0} }, { {1,0}, {1,0} },
        { {0,1}, {0,1} }, { {1,1}, {1,1} }
    };
    m_quadVertexBuffer.Attach(new GraphicsBuffer(GraphicsBuffer::Target::Vertex, GraphicsBuffer::UsageFlags::None, 4, sizeof(QV), verts));

    uint16_t indices[6] = { 0, 1, 2, 2, 1, 3 };
    m_quadIndexBuffer.Attach(new GraphicsBuffer(GraphicsBuffer::Target::Index, GraphicsBuffer::UsageFlags::None, 6, sizeof(uint16_t), indices));
}

void UICanvasSystem::CalculateRectTransforms(World& world, Entity currentEntity, const Vector2& parentRectMin, const Vector2& parentRectMax)
{
    RectTransform* rect = world.GetComponent<RectTransform>(currentEntity);
    Vector2 currentMin = parentRectMin;
    Vector2 currentMax = parentRectMax;

    if (rect)
    {
        float pWidth = parentRectMax.x - parentRectMin.x;
        float pHeight = parentRectMax.y - parentRectMin.y;

        float anchorMinX = parentRectMin.x + pWidth * rect->anchorMin.x;
        float anchorMinY = parentRectMin.y + pHeight * rect->anchorMin.y;
        float anchorMaxX = parentRectMin.x + pWidth * rect->anchorMax.x;
        float anchorMaxY = parentRectMin.y + pHeight * rect->anchorMax.y;

        float posX = anchorMinX + rect->anchoredPosition.x;
        float posY = anchorMinY + rect->anchoredPosition.y;

        rect->rectMin.x = posX - rect->pivot.x * rect->sizeDelta.x;
        rect->rectMin.y = posY - rect->pivot.y * rect->sizeDelta.y;

        rect->rectMax.x = rect->rectMin.x + rect->sizeDelta.x + (anchorMaxX - anchorMinX);
        rect->rectMax.y = rect->rectMin.y + rect->sizeDelta.y + (anchorMaxY - anchorMinY);

        currentMin = rect->rectMin;
        currentMax = rect->rectMax;
    }

    Transform* transform = world.GetComponent<Transform>(currentEntity);
    if (!transform) return;

    Entity childEntity = transform->firstChild;
    while (childEntity.id != (EntitySize)-1)
    {
        if (world.IsAlive(childEntity))
        {
            CalculateRectTransforms(world, childEntity, currentMin, currentMax);
        }
        Transform* cTrans = world.GetComponent<Transform>(childEntity);
        childEntity = cTrans ? cTrans->nextSibling : Entity{ (EntitySize)-1 };
    }
}

void UICanvasSystem::CollectUIChildren(World& world, Transform* parentTransform, std::vector<UIDrawItem>& outItems)
{
    Entity childEntity = parentTransform->firstChild;

    while (childEntity.id != (EntitySize)-1)
    {
        if (!world.IsAlive(childEntity))
            break;

        Transform* childTransform = world.GetComponent<Transform>(childEntity);
        if (!childTransform)
            break;

        RectTransform* rect = world.GetComponent<RectTransform>(childEntity);
        UIGraphic* graphic = world.GetComponent<UIGraphic>(childEntity);

        if (rect && graphic && graphic->isEnabled)
        {
            UIDrawItem item;
            item.entity = childEntity;
            item.rect = rect;
            item.graphic = graphic;
            item.depth = graphic->depth;
            outItems.push_back(item);
        }

        CollectUIChildren(world, childTransform, outItems);
        childEntity = childTransform->nextSibling;
    }
}

void UICanvasSystem::Update(World& world)
{
    float sw = (float)Screen::GetWidth();
    float sh = (float)Screen::GetHeight();

    View<Canvas> canvasView(world);
    for (auto [canvasEntity, canvas] : canvasView)
    {
        if (canvas.renderMode != RenderMode::ScreenSpaceOverlay)
            continue;

        Transform* canvasTransform = world.GetComponent<Transform>(canvasEntity);
        if (!canvasTransform) continue;

        Vector2 canvasMin(0.0f, 0.0f);
        Vector2 canvasMax(sw, sh);

        CalculateRectTransforms(world, canvasEntity, canvasMin, canvasMax);
    }
}

void UICanvasSystem::DrawRect(
    ID3D12GraphicsCommandList* cmdList,
    UINT& objIndex,
    UINT frameOffset,
    UINT slotSize,
    D3D12_GPU_VIRTUAL_ADDRESS gpuBase,
    float posX, float posY,
    float width, float height,
    const Color& color,
    Sprite* sprite
)
{
    if (objIndex >= MAX_UI_PER_FRAME) return;

    ConstantBufferLayout::Object obj;
    memset(&obj, 0, sizeof(obj));
    obj.world.m[0][0] = width;
    obj.world.m[1][1] = height;
    obj.world.m[2][2] = 1.0f;
    obj.world.m[3][0] = posX;
    obj.world.m[3][1] = posY;
    obj.world.m[3][3] = 1.0f;
    obj.world = obj.world.Transpose();
    obj.color = color;

    if (sprite && sprite->GetTexture())
    {
        obj.minUV.x = sprite->GetMinUV().x;
        obj.maxUV.x = sprite->GetMaxUV().x;

        obj.minUV.y = sprite->GetMaxUV().y;
        obj.maxUV.y = sprite->GetMinUV().y;

        obj.textureIndex = sprite->GetTexture()->GetBindlessIndex();
        obj.hasTexture = 1;
    }
    else
    {
        obj.minUV = Vector2(0, 0);
        obj.maxUV = Vector2(1, 1);
        obj.textureIndex = 0;
        obj.hasTexture = 0;
    }

    UINT slotIndex = frameOffset + 1 + objIndex;
    memcpy(m_mappedConstants + slotIndex * slotSize, &obj, sizeof(obj));

    cmdList->SetGraphicsRootConstantBufferView(1, gpuBase + slotIndex * slotSize); // Register b1
    cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);
    objIndex++;
}

void UICanvasSystem::Draw(World& world)
{
    View<Canvas> canvasView(world);
    if (canvasView.Empty()) return;

    Shader* uiShader = ShaderRegistry::GetShader("ScreenSpaceOverlay");
    if (!uiShader) return;

    ID3D12GraphicsCommandList* cmdList = Graphics::GetCurrentFrameResource()->GetCommandList();
    const UINT frameIndex = Graphics::GetCurrentFrameResource()->GetFrameIndex();
    const UINT slotSize = m_constantBufferRing->GetStride();
    const UINT slotsPerFrame = 1 + MAX_UI_PER_FRAME;
    const UINT frameOffset = frameIndex * slotsPerFrame;
    const D3D12_GPU_VIRTUAL_ADDRESS gpuBase = m_constantBufferRing->GetNativeBufferPtr()->GetGPUVirtualAddress();

    float sw = (float)Screen::GetWidth();
    float sh = (float)Screen::GetHeight();

    ConstantBufferLayout::Ortho ortho;
    memset(&ortho, 0, sizeof(ortho));
    ortho.orthoMatrix.m[0][0] = 2.0f / sw;
    ortho.orthoMatrix.m[1][1] = -2.0f / sh;
    ortho.orthoMatrix.m[2][2] = 1.0f;
    ortho.orthoMatrix.m[3][0] = -1.0f;
    ortho.orthoMatrix.m[3][1] = 1.0f;
    ortho.orthoMatrix.m[3][3] = 1.0f;
    ortho.orthoMatrix = ortho.orthoMatrix.Transpose();

    memcpy(m_mappedConstants + (frameOffset + 0) * slotSize, &ortho, sizeof(ortho));
    D3D12_GPU_VIRTUAL_ADDRESS orthoGpu = gpuBase + (frameOffset + 0) * slotSize;

    cmdList->SetPipelineState(uiShader->GetPSO());
    cmdList->SetGraphicsRootSignature(ShaderRegistry::GetRootSignature());

    ID3D12DescriptorHeap* heaps[] = { BindlessHeap::GetInstance()->GetHeap()->GetNativeHeapPointer()};
    cmdList->SetDescriptorHeaps(_countof(heaps), heaps);

    // Bind Root constants
    cmdList->SetGraphicsRootConstantBufferView(0, orthoGpu); // Register b0
    cmdList->SetGraphicsRootDescriptorTable(5, BindlessHeap::GetInstance()->GetHeap()->GetGPUDescriptorHandle(0)); // Register t0[] at space1 (Root Parameter 5 in ShaderRegistry)

    cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    D3D12_VERTEX_BUFFER_VIEW vbv = {};
    vbv.BufferLocation = m_quadVertexBuffer->GetNativeBufferPtr()->GetGPUVirtualAddress();
    vbv.StrideInBytes = m_quadVertexBuffer->GetStride();
    vbv.SizeInBytes = m_quadVertexBuffer->GetSizeInBytes();
    cmdList->IASetVertexBuffers(0, 1, &vbv);

    D3D12_INDEX_BUFFER_VIEW ibv = {};
    ibv.BufferLocation = m_quadIndexBuffer->GetNativeBufferPtr()->GetGPUVirtualAddress();
    ibv.SizeInBytes = m_quadIndexBuffer->GetSizeInBytes();
    ibv.Format = DXGI_FORMAT_R16_UINT;
    cmdList->IASetIndexBuffer(&ibv);

    UINT objIndex = 0;

    struct CanvasEntry
    {
        Entity entity;
        Canvas* canvas;
    };

    std::vector<CanvasEntry> canvases;
    for (auto [e, c] : canvasView) canvases.push_back({ e, &c });
    std::sort(canvases.begin(), canvases.end(), [](const CanvasEntry& a, const CanvasEntry& b) {
        return a.canvas->sortingOrder < b.canvas->sortingOrder;
        });

    for (auto& ce : canvases)
    {
        if (ce.canvas->renderMode != RenderMode::ScreenSpaceOverlay) continue;

        Transform* canvasTransform = world.GetComponent<Transform>(ce.entity);
        if (!canvasTransform) continue;

        std::vector<UIDrawItem> items;
        CollectUIChildren(world, canvasTransform, items);

        std::sort(items.begin(), items.end(), [](const UIDrawItem& a, const UIDrawItem& b) {
            return a.depth < b.depth;
            });

        for (auto& item : items)
        {
            RectTransform* rect = item.rect;
            float offX = rect->rectMin.x;
            float offY = rect->rectMin.y;
            float width = rect->rectMax.x - rect->rectMin.x;
            float height = rect->rectMax.y - rect->rectMin.y;

            Slider* slider = world.GetComponent<Slider>(item.entity);
            if (slider)
            {
                DrawRect(cmdList, objIndex, frameOffset, slotSize, gpuBase,
                    offX, offY, width, height, slider->backgroundColor, slider->backgroundSprite.Get());

                if (slider->isLeftToRight)
                {
                    float normValue = (slider->value - slider->minValue) / (slider->maxValue - slider->minValue);
                    normValue = (normValue < 0.0f) ? 0.0f : ((normValue > 1.0f) ? 1.0f : normValue);
                    float fillWidth = width * normValue;
                    DrawRect(cmdList, objIndex, frameOffset, slotSize, gpuBase,
                        offX, offY, fillWidth, height, slider->fillColor, slider->fillSprite.Get());
                }
                else
                {
                    float normValue = (slider->value - slider->minValue) / (slider->maxValue - slider->minValue);
                    normValue = (normValue < 0.0f) ? 0.0f : ((normValue > 1.0f) ? 1.0f : normValue);
                    float fillWidth = width * normValue;
                    DrawRect(cmdList, objIndex, frameOffset, slotSize, gpuBase,
						offX + width - fillWidth, offY, fillWidth, height, slider->fillColor, slider->fillSprite.Get());
                }

                continue;
            }

			Button* button = world.GetComponent<Button>(item.entity);
            if (button)
            {
                Color btnColor;
                switch (button->state)
                {
                case UIButtonState::Normal: btnColor = button->normalColor; break;
                case UIButtonState::Hovered: btnColor = button->hoveredColor; break;
                case UIButtonState::Selected: btnColor = button->selectedColor; break;
                case UIButtonState::Pressed: btnColor = button->pressedColor; break;
                case UIButtonState::Disabled: btnColor = button->disabledColor; break;
                default: btnColor = button->normalColor; break;
                }
                DrawRect(cmdList, objIndex, frameOffset, slotSize, gpuBase,
                    offX, offY, width, height, btnColor, button->sprite.Get());
                continue;
			}

            Image* image = world.GetComponent<Image>(item.entity);
            Sprite* sprite = (image && image->sprite) ? image->sprite.Get() : nullptr;

            DrawRect(cmdList, objIndex, frameOffset, slotSize, gpuBase,
                offX, offY, width, height, item.graphic->color, sprite);
        }
    }
}