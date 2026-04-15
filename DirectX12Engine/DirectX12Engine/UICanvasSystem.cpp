#include "UICanvasSystem.h"

struct UICanvasSystem::ConstantBufferLayout
{
    // Slot 0 (b0): ortho
    struct Ortho { Matrix4x4 orthoMatrix; };

    // Slot 1 (b1): object
    struct Object
    {
        Matrix4x4    world;
        Color        color;
        Vector2      minUV;
        Vector2      maxUV;
        unsigned int textureIndex;
        unsigned int padding[3];
    };

    struct QuadVertex { Vector2 position; Vector2 uv; };
};

void UICanvasSystem::Start(World& world)
{
    ID3D12Device* device = Graphics::GetD3D12Device();

    ComPtr<ShaderBytecode> vs;
    vs.Attach(new ShaderBytecode(L"UICanvas.hlsl", "VSMain", "vs_5_1"));

    ComPtr<ShaderBytecode> ps;
    ps.Attach(new ShaderBytecode(L"UICanvas.hlsl", "PSMain", "ps_5_1"));

    // --- RootSignature ---
    D3D12_ROOT_PARAMETER rootParams[3];
    memset(rootParams, 0, sizeof(rootParams));

    rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParams[0].Descriptor.ShaderRegister = 0;
    rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParams[1].Descriptor.ShaderRegister = 1;
    rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    // BindlessHeapからテクスチャ配列を参照
    D3D12_DESCRIPTOR_RANGE range = {};
    range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    range.BaseShaderRegister = 0;
    range.RegisterSpace = 0;
    range.NumDescriptors = BindlessHeap::MAX_DESCRIPTORS;
    range.OffsetInDescriptorsFromTableStart = 0;

    rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParams[2].DescriptorTable.NumDescriptorRanges = 1;
    rootParams[2].DescriptorTable.pDescriptorRanges = &range;
    rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    D3D12_STATIC_SAMPLER_DESC sampler = {};
    sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    sampler.MaxLOD = D3D12_FLOAT32_MAX;

    D3D12_ROOT_SIGNATURE_DESC rsDesc = {};
    rsDesc.NumParameters = _countof(rootParams);
    rsDesc.pParameters = rootParams;
    rsDesc.NumStaticSamplers = 1;
    rsDesc.pStaticSamplers = &sampler;
    rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ComPtr<ID3DBlob> blob, errBlob;
    D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, blob.ReleaseAndGetAddressOf(), errBlob.ReleaseAndGetAddressOf());
    if (errBlob) { OutputDebugStringA((const char*)errBlob->GetBufferPointer()); assert(0); }
    device->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(), IID_ID3D12RootSignature, (void**)m_rootSignature.ReleaseAndGetAddressOf());

    // --- PSO ---
    static const D3D12_INPUT_ELEMENT_DESC inputElems[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso = {};
    pso.InputLayout.NumElements = _countof(inputElems);
    pso.InputLayout.pInputElementDescs = inputElems;
    pso.VS = { vs->GetBytecodePointer(), vs->GetBytecodeLength() };
    pso.PS = { ps->GetBytecodePointer(), ps->GetBytecodeLength() };
    pso.pRootSignature = m_rootSignature.Get();
    pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pso.SampleDesc.Count = 1;
    pso.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
    pso.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    pso.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    pso.NumRenderTargets = 1;
    pso.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    pso.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    pso.DepthStencilState.DepthEnable = FALSE;
    pso.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
    pso.BlendState.RenderTarget[0].BlendEnable = TRUE;
    pso.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    pso.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
    pso.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    pso.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    pso.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
    pso.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    pso.BlendState.RenderTarget[0].RenderTargetWriteMask = 0x0f;

    device->CreateGraphicsPipelineState(&pso, IID_ID3D12PipelineState, (void**)m_pipelineState.ReleaseAndGetAddressOf());

    // --- リングバッファ ---
    const UINT slotSize = (sizeof(ConstantBufferLayout::Object) + 255) & ~255;
    const UINT totalSlots = (1 + MAX_UI_PER_FRAME) * Graphics::BackBafferCount;
    m_constantBufferRing.Attach(new GraphicsBuffer(
        GraphicsBuffer::Target::Constant,
        GraphicsBuffer::UsageFlags::LockBufferForWrite,
        totalSlots,
        slotSize
    ));
    m_mappedConstants = (BYTE*)m_constantBufferRing->LockBufferForWrite();

    // 単位クワッドの作成
    struct QV { Vector2 p; Vector2 uv; };
    QV verts[4] = {
        { {0,0}, {0,0} }, { {1,0}, {1,0} },
        { {0,1}, {0,1} }, { {1,1}, {1,1} }
    };
    m_quadVertexBuffer.Attach(new GraphicsBuffer(GraphicsBuffer::Target::Vertex, GraphicsBuffer::UsageFlags::None, 4, sizeof(QV), verts));

    uint16_t indices[6] = { 0, 1, 2, 2, 1, 3 };
    m_quadIndexBuffer.Attach(new GraphicsBuffer(GraphicsBuffer::Target::Index, GraphicsBuffer::UsageFlags::None, 6, sizeof(uint16_t), indices));
}

void UICanvasSystem::Draw(World& world)
{
    View<UIImage> view(world);
    bool hasAny = false;
    for (auto [e, img] : view) { hasAny = true; break; }
    if (!hasAny) return;

    ID3D12GraphicsCommandList* cmdList = Graphics::GetCurrentFrameResource()->GetCommandList();
    const UINT frameIndex = Graphics::GetCurrentFrameResource()->GetFrameIndex();
    const UINT slotSize = m_constantBufferRing->GetStride();
    const UINT slotsPerFrame = 1 + MAX_UI_PER_FRAME;
    const UINT frameOffset = frameIndex * slotsPerFrame;
    const D3D12_GPU_VIRTUAL_ADDRESS gpuBase = m_constantBufferRing->GetNativeBufferPtr()->GetGPUVirtualAddress();

    // 正射影行列
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

    cmdList->SetPipelineState(m_pipelineState.Get());
    cmdList->SetGraphicsRootSignature(m_rootSignature.Get());

    // BindlessHeapを共有（グローバルヒープを使用！）
    ID3D12DescriptorHeap* heaps[] = { BindlessHeap::GetInstance()->GetHeap()->GetNativeHeapPointer() };
    cmdList->SetDescriptorHeaps(_countof(heaps), heaps);

    cmdList->SetGraphicsRootConstantBufferView(0, orthoGpu);
    cmdList->SetGraphicsRootDescriptorTable(2, BindlessHeap::GetInstance()->GetHeap()->GetGPUDescriptorHandle(0));
    cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // 共用クワッド
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
    for (auto [entity, image] : view)
    {
        if (!image.isEnabled) continue;
        if (!image.sprite) continue;
        if (!image.sprite->GetTexture()) continue;
        if (objIndex >= MAX_UI_PER_FRAME) break;

        // テクスチャはBindlessHeapに既に登録済み（TextureImporterで登録）
        UINT texIndex = image.sprite->GetTexture()->GetBindlessIndex();

        float offX = image.position.x - image.pivot.x * image.size.x;
        float offY = image.position.y - image.pivot.y * image.size.y;

        ConstantBufferLayout::Object obj;
        memset(&obj, 0, sizeof(obj));
        obj.world.m[0][0] = image.size.x;
        obj.world.m[1][1] = image.size.y;
        obj.world.m[2][2] = 1.0f;
        obj.world.m[3][0] = offX;
        obj.world.m[3][1] = offY;
        obj.world.m[3][3] = 1.0f;
        obj.world = obj.world.Transpose();
        obj.color = image.color;
        obj.minUV = image.sprite->GetMinUV();
        obj.maxUV = image.sprite->GetMaxUV();
        obj.textureIndex = texIndex;

        UINT slotIndex = frameOffset + 1 + objIndex;
        memcpy(m_mappedConstants + slotIndex * slotSize, &obj, sizeof(obj));

        cmdList->SetGraphicsRootConstantBufferView(1, gpuBase + slotIndex * slotSize);
        cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);
        objIndex++;
    }
}