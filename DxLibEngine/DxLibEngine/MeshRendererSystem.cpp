#include "MeshRendererSystem.h"
#include "TransformSystem.h"
#include "CameraSystem.h"

// CHANGED: シェーダーに渡す定数バッファのレイアウトを拡張
struct MeshRendererSystem::ConstantBufferLayout
{
    Matrix4x4 worldMatrix;
    Color     diffuseColor;
    Color     specularColor;
};

// このシステムで共有するグラフィックスリソースを初期化します
void MeshRendererSystem::StaticConstructor()
{
    ID3D12Device* d3d12Device = Graphics::GetD3D12Device();

    // シェーダーのコンパイル (変更なし)
    ComPtr<ShaderBytecode> vertexShader;
    vertexShader.Attach(new ShaderBytecode(L"MeshRenderer.hlsl", "VSMain", "vs_5_1"));

    ComPtr<ShaderBytecode> pixelShader;
    pixelShader.Attach(new ShaderBytecode(L"MeshRenderer.hlsl", "PSMain", "ps_5_1"));

    // ルートシグネチャをマルチテクスチャ対応に
    D3D12_ROOT_PARAMETER rootParameters[3];
    memset(rootParameters, 0, sizeof(rootParameters));

    // register(b0): カメラ定数バッファ (変更なし)
    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[0].Descriptor.ShaderRegister = 0;
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    // register(b1): オブジェクト・マテリアル定数バッファ (変更なし)
    rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[1].Descriptor.ShaderRegister = 1;
    rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    // ディスクリプタテーブル: 複数のテクスチャ (Diffuse, Normal, Specular) を設定
    D3D12_DESCRIPTOR_RANGE ranges[1];
    memset(ranges, 0, sizeof(ranges));
    ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

    // ここでデスクリプタの数をMaterial::TextureSlotの最大数に設定
    ranges[0].NumDescriptors = (UINT)Material::TextureSlot::Max;
    ranges[0].BaseShaderRegister = 0; // register(t0) から始まる
    ranges[0].RegisterSpace = 0;
    ranges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(ranges);
    rootParameters[2].DescriptorTable.pDescriptorRanges = ranges;
    rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    // 静的サンプラー
    D3D12_STATIC_SAMPLER_DESC sampler = {};
    sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    sampler.MaxLOD = D3D12_FLOAT32_MAX;
    sampler.ShaderRegister = 0; // register(s0)
    sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    // ルートシグネチャのデスクリプタを完成させる
    D3D12_ROOT_SIGNATURE_DESC rsDesc = {};
    rsDesc.NumParameters = _countof(rootParameters);
    rsDesc.pParameters = rootParameters;
    rsDesc.pStaticSamplers = &sampler;
    rsDesc.NumStaticSamplers = 1;
    rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    // ルートシグネチャをシリアライズ
    ComPtr<ID3DBlob> serializedRootSignature;
    ComPtr<ID3DBlob> errorBlob;
    HRESULT hr = D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &serializedRootSignature, &errorBlob);
    if (FAILED(hr))
    {
        if (errorBlob)
        {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        }
        assert(SUCCEEDED(hr));
    }

    // デバイスからルートシグネチャを作成
    hr = d3d12Device->CreateRootSignature(0, serializedRootSignature->GetBufferPointer(), serializedRootSignature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));
    assert(SUCCEEDED(hr));

    // 入力レイアウトにTANGENTを追加
    static const D3D12_INPUT_ELEMENT_DESC inputElements[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // TANGENT を追加
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
    // StaticConstructorで作成した静的リソースを解放します。
    // ComPtrが自動的にRelease()を呼ぶため、nullptrを代入するだけで十分です。
    m_graphicsPipelineState = nullptr;
    m_rootSignature = nullptr;
    m_defaultWhiteTexture = nullptr;
}

void MeshRendererSystem::Start(ComponentManager& cm, World& world)
{
    TextureImporter importer;

    // TextureImporterを使わずに手動でTexture2Dオブジェクトを作成
    m_defaultWhiteTexture.Attach(importer.Import(L"Assets/White.png"));
}

void MeshRendererSystem::Draw(ComponentManager& cm, World& world)
{
    View<Transform, MeshRenderer> view(cm);
    ID3D12GraphicsCommandList* commandList = Graphics::GetCurrentFrameResource()->GetCommandList();

    // 共通の描画設定
    commandList->SetPipelineState(m_graphicsPipelineState.Get());
    commandList->SetGraphicsRootSignature(m_rootSignature.Get());

    // 共通のカメラ情報
    CameraSystem* cameraSystem = world.GetSystem<CameraSystem>();
    Camera* camera = cameraSystem->GetCurrent();
    if (!camera) return;
    commandList->SetGraphicsRootConstantBufferView(0, cameraSystem->GetCameraBuffer(*camera)->GetNativeBufferPtr()->GetGPUVirtualAddress());

    // 描画対象のエンティティをループ
    for (auto [entity, transform, renderer] : view)
    {
        // レンダラーがメッシュを持っていない場合はスキップ
        if (renderer.meshes.empty()) continue;

        // ワールド行列は、このエンティティの全メッシュで共通
        const Matrix4x4& worldMatrix = world.GetSystem<TransformSystem>()->GetLocalToWorldMatrix(transform);

        // このエンティティが持つ全てのメッシュを描画するループ
        for (const auto& meshComponent : renderer.meshes)
        {
            Mesh* mesh = meshComponent.Get();
            if (!mesh) continue;

            // このメッシュが使用するマテリアルを取得
            int materialIndex = mesh->GetMaterialIndex();
            if (materialIndex < 0 || materialIndex >= renderer.materials.size()) continue; // 安全チェック

            Material* material = renderer.materials[materialIndex].Get();
            if (!material) continue;

            // このメッシュ・マテリアル用の定数バッファがなければ作成
            if (!renderer.constantBuffer) // NOTE: 本来はメッシュ毎に持つべきだが、簡略化のためRendererで一つ持つ
            {
                renderer.constantBuffer.Attach(new GraphicsBuffer(
                    GraphicsBuffer::Target::Constant,
                    GraphicsBuffer::UsageFlags::LockBufferForWrite,
                    1, sizeof(ConstantBufferLayout)
                ));
            }

            // 定数バッファの内容を更新
            ConstantBufferLayout* lockedPointer = (ConstantBufferLayout*)renderer.constantBuffer->LockBufferForWrite();
            lockedPointer->worldMatrix = worldMatrix.Transpose();
            lockedPointer->diffuseColor = material->GetDiffuseColor();   // マテリアルから拡散反射色を取得
            lockedPointer->specularColor = material->GetSpecularColor(); // マテリアルから鏡面反射色を取得
            renderer.constantBuffer->UnlockBufferAfterWrite();

            // オブジェクト・マテリアル固有の定数バッファをシェーダーにセット
            commandList->SetGraphicsRootConstantBufferView(1, renderer.constantBuffer->GetNativeBufferPtr()->GetGPUVirtualAddress());

            // マテリアルのデスクリプタヒープをシェーダーにセット
            ID3D12DescriptorHeap* descriptorHeap = material->GetDescriptorHeap();
            if (descriptorHeap)
            {
                commandList->SetDescriptorHeaps(1, &descriptorHeap);
                // ディスクリプタテーブルの開始ハンドルをセット
                commandList->SetGraphicsRootDescriptorTable(2, material->GetGpuDescriptorHandle(Material::TextureSlot::Diffuse));
            }

            // メッシュのジオメトリ情報をIAステージにセット
            GraphicsBuffer* vertexBuffer = mesh->GetVertexBuffer();
            D3D12_VERTEX_BUFFER_VIEW vbView = {};
            vbView.BufferLocation = vertexBuffer->GetNativeBufferPtr()->GetGPUVirtualAddress();
            vbView.StrideInBytes = vertexBuffer->GetStride();
            vbView.SizeInBytes = vertexBuffer->GetSizeInBytes();
            commandList->IASetVertexBuffers(0, 1, &vbView);

            GraphicsBuffer* indexBuffer = mesh->GetIndexBuffer();
            D3D12_INDEX_BUFFER_VIEW ibView = {};
            ibView.BufferLocation = indexBuffer->GetNativeBufferPtr()->GetGPUVirtualAddress();
            ibView.SizeInBytes = indexBuffer->GetSizeInBytes();
            ibView.Format = DXGI_FORMAT_R32_UINT;
            commandList->IASetIndexBuffer(&ibView);

            commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            // 描画！
            commandList->DrawIndexedInstanced(mesh->GetIndexCount(), 1, 0, 0, 0);
        }
    }
}