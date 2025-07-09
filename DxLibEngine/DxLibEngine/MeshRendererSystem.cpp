#include "MeshRendererSystem.h"
#include "TransformSystem.h"
#include "CameraSystem.h"

// 定数バッファのメモリレイアウト
struct MeshRendererSystem::ConstantBufferLayout
{
    Matrix4x4 worldMatrix;
    Color color;
};

// このシステムで共有するグラフィックスリソースを初期化します
void MeshRendererSystem::StaticConstructor()
{
    ID3D12Device* d3d12Device = Graphics::GetD3D12Device();

    // 3D用のシェーダーをコンパイルします
    ComPtr<ShaderBytecode> vertexShader;
    vertexShader.Attach(new ShaderBytecode(L"MeshRenderer.hlsl", "VSMain", "vs_5_1"));

    ComPtr<ShaderBytecode> pixelShader;
    pixelShader.Attach(new ShaderBytecode(L"MeshRenderer.hlsl", "PSMain", "ps_5_1"));

    // ルートシグネチャを作成します
    D3D12_ROOT_PARAMETER rootParameters[3];
    memset(rootParameters, 0, sizeof(rootParameters));

    // register(b0): カメラ定数バッファ
    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[0].Descriptor.ShaderRegister = 0;
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    // register(b1): オブジェクト定数バッファ
    rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[1].Descriptor.ShaderRegister = 1;
    rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    // ディスクリプタテーブルの作成
    D3D12_DESCRIPTOR_RANGE ranges[1];
    memset(ranges, 0, sizeof(ranges));
    ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // シェーダーリソースビュー
    ranges[0].NumDescriptors = 1;                          // テクスチャは1つ
    ranges[0].BaseShaderRegister = 0;                      // register(t0) に対応
    ranges[0].RegisterSpace = 0;
    ranges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(ranges);
    rootParameters[2].DescriptorTable.pDescriptorRanges = ranges;
    rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // ピクセルシェーダーで使う

    D3D12_STATIC_SAMPLER_DESC sampler = {};
    sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイリニアフィルタ
    sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    sampler.MaxLOD = D3D12_FLOAT32_MAX;
    sampler.ShaderRegister = 0; // register(s0) に対応
    sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    D3D12_ROOT_SIGNATURE_DESC rsDesc = {};
    rsDesc.NumParameters = _countof(rootParameters);
    rsDesc.pParameters = rootParameters;
    rsDesc.pStaticSamplers = &sampler;
    rsDesc.NumStaticSamplers = 1;
    rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ComPtr<ID3DBlob> serializedRootSignature;
    ComPtr<ID3DBlob> errorBlob;
    if (FAILED(D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, serializedRootSignature.ReleaseAndGetAddressOf(), errorBlob.ReleaseAndGetAddressOf())))
    {
        if (errorBlob) OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        assert(0);
    }
    if (FAILED(d3d12Device->CreateRootSignature(0, serializedRootSignature->GetBufferPointer(), serializedRootSignature->GetBufferSize(), IID_PPV_ARGS(m_rootSignature.ReleaseAndGetAddressOf()))))
    {
        assert(0);
    }

    // 入力レイアウトを定義します (Mesh.hのVertex構造体と一致させる)
    static const D3D12_INPUT_ELEMENT_DESC inputElements[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    // グラフィックスパイプラインステートを作成します (PSO)
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.pRootSignature = m_rootSignature.Get();
    psoDesc.VS = { vertexShader->GetBytecodePointer(), vertexShader->GetBytecodeLength() };
    psoDesc.PS = { pixelShader->GetBytecodePointer(), pixelShader->GetBytecodeLength() };
    psoDesc.InputLayout = { inputElements, _countof(inputElements) };
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    psoDesc.SampleDesc.Count = 1;
    psoDesc.SampleMask = UINT_MAX;

    // ラスタライザーステート: 裏面をカリング（描画しない）します
    psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;

    // 深度テストを有効にします
    psoDesc.DepthStencilState.DepthEnable = TRUE;
    psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    psoDesc.DepthStencilState.StencilEnable = FALSE;

    // ブレンドステート: 半透明を有効にします
    psoDesc.BlendState.RenderTarget[0].BlendEnable = TRUE;
    psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
    psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
    psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    if (FAILED(d3d12Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(m_graphicsPipelineState.ReleaseAndGetAddressOf()))))
    {
        assert(0);
    }
}

void MeshRendererSystem::StaticDestructor()
{
    m_graphicsPipelineState = nullptr;
    m_rootSignature = nullptr;
}

void MeshRendererSystem::SetMesh(MeshRenderer* renderer, Mesh* mesh)
{
    if (renderer->mesh.Get() != mesh)
    {
        renderer->mesh = mesh;
    }
}

void MeshRendererSystem::Draw(ComponentManager& cm, World& world)
{
    View<Transform, MeshRenderer> view(cm);

    // フレームリソースとコマンドリストを取得
    FrameResource* frameResource = Graphics::GetCurrentFrameResource();
    ID3D12GraphicsCommandList* commandList = frameResource->GetCommandList();

    // 描画設定
    commandList->SetPipelineState(m_graphicsPipelineState.Get());
    commandList->SetGraphicsRootSignature(m_rootSignature.Get());

    // カメラバッファをセット
    CameraSystem* cameraSystem = world.GetSystem<CameraSystem>();
    Camera* camera = cameraSystem->GetCurrent();
    if (!camera) return; // カメラがなければ描画しない
    commandList->SetGraphicsRootConstantBufferView(0, cameraSystem->GetCameraBuffer(*camera)->GetNativeBufferPtr()->GetGPUVirtualAddress());

    // エンティティをループして描画
    for (auto [entity, transform, renderer] : view)
    {
        if (!renderer.material || !renderer.mesh) continue;

        // 定数バッファの初期化
        if (!renderer.isStarted)
        {
            renderer.constantBuffer.Attach(new GraphicsBuffer(GraphicsBuffer::Target::Constant, GraphicsBuffer::UsageFlags::LockBufferForWrite, 1, sizeof(ConstantBufferLayout)));
            renderer.isStarted = true;
        }

        // 定数バッファを更新
        const Matrix4x4& worldMatrix = world.GetSystem<TransformSystem>()->GetLocalToWorldMatrix(transform);
        ConstantBufferLayout* lockedPointer = (ConstantBufferLayout*)renderer.constantBuffer->LockBufferForWrite();
        lockedPointer->worldMatrix = worldMatrix.Transpose();
        lockedPointer->color = renderer.material.Get()->GetColor();
        renderer.constantBuffer->UnlockBufferAfterWrite();

        // オブジェクトの定数バッファをセット
        commandList->SetGraphicsRootConstantBufferView(1, renderer.constantBuffer->GetNativeBufferPtr()->GetGPUVirtualAddress());

        if (renderer.material->GetMainTexture())
        {
            // ディスクリプタヒープをセット
            ID3D12DescriptorHeap* heaps[] = { renderer.material->GetDescriptorHeap()->GetNativeHeapPointer() };
            commandList->SetDescriptorHeaps(_countof(heaps), heaps);

            // ルートパラメータ2番にディスクリプタテーブルをセット
            commandList->SetGraphicsRootDescriptorTable(2, renderer.material->GetDescriptorHeap()->GetGPUDescriptorHandle(0));
        }

        // IAステージにジオメトリをセット
        GraphicsBuffer* vertexBuffer = renderer.mesh->GetVertexBuffer();
        D3D12_VERTEX_BUFFER_VIEW vbView;
        vbView.BufferLocation = vertexBuffer->GetNativeBufferPtr()->GetGPUVirtualAddress();
        vbView.StrideInBytes = vertexBuffer->GetStride();
        vbView.SizeInBytes = vertexBuffer->GetSizeInBytes();
        commandList->IASetVertexBuffers(0, 1, &vbView);

        GraphicsBuffer* indexBuffer = renderer.mesh->GetIndexBuffer();
        D3D12_INDEX_BUFFER_VIEW ibView;
        ibView.BufferLocation = indexBuffer->GetNativeBufferPtr()->GetGPUVirtualAddress();
        ibView.SizeInBytes = indexBuffer->GetSizeInBytes();
        ibView.Format = (indexBuffer->GetStride() == 4) ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
        commandList->IASetIndexBuffer(&ibView);

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // 描画コマンド
        commandList->DrawIndexedInstanced(renderer.mesh->GetIndexCount(), 1, 0, 0, 0);
    }
}