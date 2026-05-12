#pragma once

class UICanvasSystem : public System
{
private:
    ComPtr<GraphicsBuffer> m_constantBufferRing;
    ComPtr<GraphicsBuffer> m_quadVertexBuffer;
    ComPtr<GraphicsBuffer> m_quadIndexBuffer;
    BYTE* m_mappedConstants = nullptr;
    static constexpr UINT MAX_UI_PER_FRAME = 256;

    struct ConstantBufferLayout;

    struct UIDrawItem
    {
        Entity entity;
        RectTransform* rect;
        UIGraphic* graphic;
        int depth;
    };

    // Calculate global bounds based on parent bounds
    void CalculateRectTransforms(World& world, Entity currentEntity, const Vector2& parentRectMin, const Vector2& parentRectMax);

    // Recursively collect valid UI draw elements
    void CollectUIChildren(World& world, Transform* parentTransform, std::vector<UIDrawItem>& outItems);

    // Draw a single UI quad
    void DrawRect(
        ID3D12GraphicsCommandList* cmdList,
        UINT& objIndex,
        UINT frameOffset,
        UINT slotSize,
        D3D12_GPU_VIRTUAL_ADDRESS gpuBase,
        float posX, float posY,
        float width, float height,
        const Color& color,
        Sprite* sprite
    );

public:
    void Start(World& world) override;
    void Update(World& world) override;
    void Draw(World& world) override;
};