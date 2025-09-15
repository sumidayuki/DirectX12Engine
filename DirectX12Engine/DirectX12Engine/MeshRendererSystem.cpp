#include "MeshRendererSystem.h"
#include "TransformSystem.h"
#include "CameraSystem.h"
#include "LightSystem.h"

// 1フレームあたりに描画可能なオブジェクトの最大数
constexpr UINT MAX_OBJECTS_PER_FRAME = 2048;

// このシステムで共有するグラフィックスリソースを初期化します
void MeshRendererSystem::StaticConstructor()
{
    ID3D12Device* d3d12Device = Graphics::GetD3D12Device();

    // シェーダーのコンパイル (変更なし)
    ComPtr<ShaderBytecode> vertexShader;
    vertexShader.Attach(new ShaderBytecode(L"MeshRenderer.hlsl", "VSMain", "vs_5_1"));

    ComPtr<ShaderBytecode> pixelShader;
    pixelShader.Attach(new ShaderBytecode(L"MeshRenderer.hlsl", "PSMain", "ps_5_1"));

    // ルートシグネチャ (変更なし)
    D3D12_ROOT_PARAMETER rootParameters[5];
    memset(rootParameters, 0, sizeof(rootParameters));

    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[0].Descriptor.ShaderRegister = 0;
    rootParameters[0].Descriptor.RegisterSpace = 0;
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[1].Descriptor.ShaderRegister = 1;
    rootParameters[1].Descriptor.RegisterSpace = 0;
    rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    D3D12_DESCRIPTOR_RANGE ranges[1];
    memset(ranges, 0, sizeof(ranges));
    ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    ranges[0].NumDescriptors = (UINT)Material::TextureSlot::Max;
    ranges[0].BaseShaderRegister = 0;
    ranges[0].RegisterSpace = 0;
    ranges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(ranges);
    rootParameters[2].DescriptorTable.pDescriptorRanges = ranges;
    rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    D3D12_DESCRIPTOR_RANGE lightRanges[1];
    memset(lightRanges, 0, sizeof(lightRanges));
    lightRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    lightRanges[0].NumDescriptors = 1;
    lightRanges[0].BaseShaderRegister = 3;
    lightRanges[0].RegisterSpace = 0;
    lightRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[3].DescriptorTable.NumDescriptorRanges = _countof(lightRanges);
    rootParameters[3].DescriptorTable.pDescriptorRanges = lightRanges;
    rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[4].Descriptor.ShaderRegister = 2;
    rootParameters[4].Descriptor.RegisterSpace = 0;
    rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    D3D12_STATIC_SAMPLER_DESC sampler = {};
    sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    sampler.MaxLOD = D3D12_FLOAT32_MAX;
    sampler.ShaderRegister = 0;
    sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    D3D12_ROOT_SIGNATURE_DESC rsDesc = {};
    rsDesc.NumParameters = _countof(rootParameters);
    rsDesc.pParameters = rootParameters;
    rsDesc.pStaticSamplers = &sampler;
    rsDesc.NumStaticSamplers = 1;
    rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

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

    hr = d3d12Device->CreateRootSignature(0, serializedRootSignature->GetBufferPointer(), serializedRootSignature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));
    assert(SUCCEEDED(hr));

    // 入力レイアウト (変更なし)
    static const D3D12_INPUT_ELEMENT_DESC inputElements[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    // グラフィックスパイプラインステート (変更なし)
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
    psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
    psoDesc.DepthStencilState.DepthEnable = TRUE;
    psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    psoDesc.DepthStencilState.StencilEnable = FALSE;
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
    // リングバッファのマップを解除
    if (m_objectConstantBufferRing && m_mappedObjectConstants)
    {
        m_objectConstantBufferRing->UnlockBufferAfterWrite();
        m_mappedObjectConstants = nullptr;
    }

    m_graphicsPipelineState = nullptr;
    m_rootSignature = nullptr;
    m_defaultWhiteTexture = nullptr;
    m_objectConstantBufferRing = nullptr;
}

void MeshRendererSystem::Start(ComponentManager& cm, World& world)
{
    TextureImporter importer;
    m_defaultWhiteTexture.Attach(importer.Import(L"Assets/White.png"));

    m_sceneConstantBuffer.Attach(new GraphicsBuffer(
        GraphicsBuffer::Target::Constant,
        GraphicsBuffer::UsageFlags::LockBufferForWrite,
        1,
        sizeof(SceneConstants)
    ));

    // リングバッファの初期化
    // D3D12の定数バッファの要件である256バイトアライメントを計算
    const UINT alignedObjectConstantsSize = (sizeof(ObjectConstantsLayout) + 255) & ~255;

    // フレームバッファ数 x オブジェクト最大数 分の領域を確保
    m_objectConstantBufferRing.Attach(new GraphicsBuffer(
        GraphicsBuffer::Target::Constant,
        GraphicsBuffer::UsageFlags::LockBufferForWrite, // CPUから書き込み可能なアップロードヒープに作成
        Graphics::BackBafferCount * MAX_OBJECTS_PER_FRAME,
        alignedObjectConstantsSize
    ));

    // バッファを永続的にマップしておく
    m_mappedObjectConstants = (BYTE*)m_objectConstantBufferRing->LockBufferForWrite();

    // インデックスを初期化
    m_currentObjectBufferIndex = 0;
}

void MeshRendererSystem::Draw(ComponentManager& cm, World& world)
{
    ID3D12GraphicsCommandList* commandList = Graphics::GetCurrentFrameResource()->GetCommandList();
    DescriptorAllocator* srvAllocator = world.GetSrvAllocator();

    commandList->SetPipelineState(m_graphicsPipelineState.Get());
    commandList->SetGraphicsRootSignature(m_rootSignature.Get());

    ID3D12DescriptorHeap* heaps[] = { srvAllocator->GetHeap() };
    commandList->SetDescriptorHeaps(_countof(heaps), heaps);

    // 共通情報のバインド
    CameraSystem* cameraSystem = world.GetSystem<CameraSystem>();
    Camera* camera = cameraSystem->GetCurrent();
    LightSystem* lightSystem = world.GetSystem<LightSystem>();
    if (!camera || !lightSystem) return;

    commandList->SetGraphicsRootConstantBufferView(0, cameraSystem->GetCameraBuffer(*camera)->GetNativeBufferPtr()->GetGPUVirtualAddress());

    // シーンCBV
    SceneConstants* sceneData = (SceneConstants*)m_sceneConstantBuffer->LockBufferForWrite();
    sceneData->activeLightCount = lightSystem->GetActiveLightCount();
    Entity* currentCameraEntity = cameraSystem->GetCurrentEntity();
    if (currentCameraEntity)
    {
        Transform* cameraTransform = world.GetComponent<Transform>(*currentCameraEntity);
        sceneData->cameraWorldPosition = Vector4(cameraTransform->position, 1.0f);
    }
    m_sceneConstantBuffer->UnlockBufferAfterWrite();
    commandList->SetGraphicsRootConstantBufferView(4, m_sceneConstantBuffer->GetNativeBufferPtr()->GetGPUVirtualAddress());

    if (lightSystem->GetActiveLightCount() > 0)
    {
        commandList->SetGraphicsRootDescriptorTable(3, lightSystem->GetLightBufferGpuHandle());
    }

    // リングバッファのセットアップ
    // フレームごとにオブジェクトインデックスをリセット
    m_currentObjectBufferIndex = 0;

    // このフレームが使用するリングバッファのセクションを計算
    const UINT frameIndex = Graphics::GetCurrentFrameResource()->GetFrameIndex();
    const UINT alignedObjectConstantsSize = m_objectConstantBufferRing->GetStride();
    const UINT bufferOffsetForFrame = frameIndex * MAX_OBJECTS_PER_FRAME;
    D3D12_GPU_VIRTUAL_ADDRESS gpuAddressBase = m_objectConstantBufferRing->GetNativeBufferPtr()->GetGPUVirtualAddress();

    // 各オブジェクトの描画ループ
    View<Transform, MeshFilter, MeshRenderer> view(cm);
    for (auto [entity, transform, meshFilter, renderer] : view)
    {
        if (!meshFilter.mesh) continue;
        Mesh* mesh = meshFilter.mesh.Get();
        if (!mesh) continue;

        const Matrix4x4& worldMatrix = world.GetSystem<TransformSystem>()->GetLocalToWorldMatrix(transform);

        GraphicsBuffer* vertexBuffer = mesh->GetVertexBuffer();
        GraphicsBuffer* indexBuffer = mesh->GetIndexBuffer();
        if (!vertexBuffer || !indexBuffer) continue;

        D3D12_VERTEX_BUFFER_VIEW vbView = {};
        vbView.BufferLocation = vertexBuffer->GetNativeBufferPtr()->GetGPUVirtualAddress();
        vbView.StrideInBytes = vertexBuffer->GetStride();
        vbView.SizeInBytes = vertexBuffer->GetSizeInBytes();
        commandList->IASetVertexBuffers(0, 1, &vbView);

        D3D12_INDEX_BUFFER_VIEW ibView = {};
        ibView.BufferLocation = indexBuffer->GetNativeBufferPtr()->GetGPUVirtualAddress();
        ibView.SizeInBytes = indexBuffer->GetSizeInBytes();
        ibView.Format = DXGI_FORMAT_R32_UINT;
        commandList->IASetIndexBuffer(&ibView);

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // サブメッシュごとに描画
        for (UINT i = 0; i < mesh->GetSubMeshCount(); ++i)
        {
            // オブジェクト数が上限を超えたら、それ以上描画しない
            if (m_currentObjectBufferIndex >= MAX_OBJECTS_PER_FRAME) {
                // ここで警告ログなどを出すと親切
                // OutputDebugStringA("Warning: Reached maximum number of drawable objects for this frame.\n");
                break;
            }

            const SubMesh& subMesh = mesh->GetSubMesh(i);
            if (subMesh.materialIndex >= renderer.materials.size()) continue;
            Material* material = renderer.materials[subMesh.materialIndex].Get();
            if (!material) continue;

            // リングバッファへのデータ書き込み
            // 定数データをローカル変数に設定
            ObjectConstantsLayout constants;
            constants.worldMatrix = worldMatrix.Transpose();
            constants.diffuseColor = material->GetDiffuseColor();
            constants.specularColor = material->GetSpecularColor();
            constants.shininess = 64.0f; // 仮

            // リングバッファの現在位置にデータをコピー
            BYTE* dest = m_mappedObjectConstants + (bufferOffsetForFrame + m_currentObjectBufferIndex) * alignedObjectConstantsSize;
            memcpy(dest, &constants, sizeof(ObjectConstantsLayout));

            // 対応するGPUアドレスを計算してルートパラメータに設定
            D3D12_GPU_VIRTUAL_ADDRESS currentGpuAddress = gpuAddressBase + (bufferOffsetForFrame + m_currentObjectBufferIndex) * alignedObjectConstantsSize;
            commandList->SetGraphicsRootConstantBufferView(1, currentGpuAddress);

            // 次のオブジェクトのためにインデックスを進める
            m_currentObjectBufferIndex++;

            // Material Textures のバインド
            D3D12_GPU_DESCRIPTOR_HANDLE textureHandle = material->GetGpuDescriptorHandle(Material::TextureSlot::Diffuse);
            if (textureHandle.ptr != 0)
            {
                commandList->SetGraphicsRootDescriptorTable(2, textureHandle);
            }

            commandList->DrawIndexedInstanced(subMesh.indexCount, 1, subMesh.startIndex, 0, 0);
        }
    }
}