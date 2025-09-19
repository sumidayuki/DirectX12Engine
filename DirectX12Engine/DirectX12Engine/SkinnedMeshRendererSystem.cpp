#include "SkinnedMeshRendererSystem.h"

// 1フレームあたりに描画可能なオブジェクトの最大数
constexpr UINT MAX_SKINNED_OBJECTS_PER_FRAME = 1024;

void SkinnedMeshRendererSystem::StaticConstructor()
{
    ID3D12Device* device = Graphics::GetD3D12Device();

    ComPtr<ShaderBytecode> vertexShader;
    vertexShader.Attach(new ShaderBytecode(L"SkinnedMeshRenderer.hlsl", "VSMain", "vs_5_1"));

    ComPtr<ShaderBytecode> pixelShader;
    pixelShader.Attach(new ShaderBytecode(L"SkinnedMeshRenderer.hlsl", "PSMain", "ps_5_1"));

    // ルートシグネチャの作成
    D3D12_ROOT_PARAMETER rootParameters[5];
    memset(rootParameters, 0, sizeof(rootParameters));

    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[0].Descriptor.ShaderRegister = 0;
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[1].Descriptor.ShaderRegister = 1;
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
    HRESULT hr = D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1, &serializedRootSignature, &errorBlob);
    if (FAILED(hr)) {
        if (errorBlob) OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        assert(SUCCEEDED(hr));
    }
    device->CreateRootSignature(0, serializedRootSignature->GetBufferPointer(), serializedRootSignature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));

    // 頂点レイアウトの定義
    static const D3D12_INPUT_ELEMENT_DESC inputElements[] =
    {
        { "POSITION",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TANGENT",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    // PSOの作成
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
    psoDesc.BlendState.RenderTarget[0].BlendEnable = TRUE;
    psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
    psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
    psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_graphicsPipelineState));
}

void SkinnedMeshRendererSystem::StaticDestructor()
{
    if (m_objectConstantBufferRing && m_mappedObjectConstants)
    {
        m_objectConstantBufferRing->UnlockBufferAfterWrite();
        m_mappedObjectConstants = nullptr;
    }
    m_graphicsPipelineState.Reset();
    m_rootSignature.Reset();
    m_objectConstantBufferRing.Reset();
}

void SkinnedMeshRendererSystem::Start(ComponentManager& cm, World& world)
{
    m_sceneConstantBuffer.Attach(new GraphicsBuffer(
        GraphicsBuffer::Target::Constant,
        GraphicsBuffer::UsageFlags::LockBufferForWrite,
        1,
        sizeof(SceneConstants)
    ));

    // 定数バッファの作成
    const UINT alignedSize = (sizeof(SkinnedObjectConstantsLayout) + 255) & ~255;
    m_objectConstantBufferRing.Attach(new GraphicsBuffer(
        GraphicsBuffer::Target::Constant,
        GraphicsBuffer::UsageFlags::LockBufferForWrite,
        Graphics::BackBafferCount * MAX_SKINNED_OBJECTS_PER_FRAME,
        alignedSize
    ));
    m_mappedObjectConstants = (BYTE*)m_objectConstantBufferRing->LockBufferForWrite();
    m_currentObjectBufferIndex = 0;
}

void SkinnedMeshRendererSystem::Draw(ComponentManager& cm, World& world)
{
    ID3D12GraphicsCommandList* commandList = Graphics::GetCurrentFrameResource()->GetCommandList();
    DescriptorAllocator* srvAllocator = world.GetSrvAllocator();

    commandList->SetPipelineState(m_graphicsPipelineState.Get());
    commandList->SetGraphicsRootSignature(m_rootSignature.Get());

    ID3D12DescriptorHeap* heaps[] = { srvAllocator->GetHeap() };
    commandList->SetDescriptorHeaps(_countof(heaps), heaps);

    auto cameraSystem = world.GetSystem<CameraSystem>();
    auto camera = cameraSystem->GetCurrent();
    auto lightSystem = world.GetSystem<LightSystem>();

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

    m_currentObjectBufferIndex = 0;

    const UINT frameIndex = Graphics::GetCurrentFrameResource()->GetFrameIndex();
    const UINT alignedObjectConstantsSize = m_objectConstantBufferRing->GetStride();
    const D3D12_GPU_VIRTUAL_ADDRESS gpuAddressBase = m_objectConstantBufferRing->GetNativeBufferPtr()->GetGPUVirtualAddress();

    View<SkinnedMeshRenderer, Transform> view(cm);
    for (auto [entity, smr, transform] : view)
    {
        if (!smr.mesh || m_currentObjectBufferIndex >= MAX_SKINNED_OBJECTS_PER_FRAME)
        {
            OutputDebugStringA("SkinnedMeshRenderer Error: Asset path is not set or empty.\n");

            continue;
        }

        Animator* animator = world.GetComponent<Animator>(*TransformSystem::GetRoot(transform)->entity);

        const Matrix4x4& worldMatrix = TransformSystem::GetLocalToWorldMatrix(transform);

        GraphicsBuffer* vertexBuffer = smr.mesh->GetVertexBuffer();
        GraphicsBuffer* indexBuffer = smr.mesh->GetIndexBuffer();
        if (!vertexBuffer || !indexBuffer) continue;

        D3D12_VERTEX_BUFFER_VIEW vbView = {};
        vbView.BufferLocation = smr.mesh->GetVertexBuffer()->GetNativeBufferPtr()->GetGPUVirtualAddress();
        vbView.StrideInBytes = smr.mesh->GetVertexBuffer()->GetStride();
        vbView.SizeInBytes = smr.mesh->GetVertexBuffer()->GetSizeInBytes();
        commandList->IASetVertexBuffers(0, 1, &vbView);

        D3D12_INDEX_BUFFER_VIEW ibView = {};
        ibView.BufferLocation = smr.mesh->GetIndexBuffer()->GetNativeBufferPtr()->GetGPUVirtualAddress();
        ibView.SizeInBytes = smr.mesh->GetIndexBuffer()->GetSizeInBytes();
        ibView.Format = DXGI_FORMAT_R32_UINT;
        commandList->IASetIndexBuffer(&ibView);

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        for (UINT i = 0; i < smr.mesh->GetSubMeshCount(); ++i)
        {
            if (m_currentObjectBufferIndex >= MAX_SKINNED_OBJECTS_PER_FRAME)
            {
                break;
            }

            const auto& submesh = smr.mesh->GetSubMesh(i);
            if (submesh.materialIndex >= smr.materials.size()) continue;
            Material* material = smr.materials[submesh.materialIndex];
            if (!material) continue;

            SkinnedObjectConstantsLayout constants;
            constants.worldMatrix = worldMatrix.Transpose();
            constants.diffuseColor = material->GetDiffuseColor();
            constants.specularColor = material->GetSpecularColor();
            constants.shininess = 64.0f;

            const size_t boneCount = std::min(animator->skeleton->GetBoneCount(), SkinnedObjectConstantsLayout::MAX_BONES);
            for (size_t j = 0; j < boneCount; ++j)
            {
                constants.boneMatrices[j] = animator->finalBoneMatrices[j].Transpose();
            }

            const UINT bufferOffsetForFrame = frameIndex * MAX_SKINNED_OBJECTS_PER_FRAME;
            BYTE* dest = m_mappedObjectConstants + (bufferOffsetForFrame + m_currentObjectBufferIndex) * alignedObjectConstantsSize;
            memcpy(dest, &constants, sizeof(SkinnedObjectConstantsLayout));

            D3D12_GPU_VIRTUAL_ADDRESS currentGpuAddres = gpuAddressBase + (bufferOffsetForFrame + m_currentObjectBufferIndex) * alignedObjectConstantsSize;
            commandList->SetGraphicsRootConstantBufferView(1, currentGpuAddres);

            m_currentObjectBufferIndex++;

            D3D12_GPU_DESCRIPTOR_HANDLE textureHandle = material->GetGpuDescriptorHandle(Material::TextureSlot::Diffuse);
            if (textureHandle.ptr != 0)
            {
                commandList->SetGraphicsRootDescriptorTable(2, textureHandle);
            }

            commandList->DrawIndexedInstanced(submesh.indexCount, 1, submesh.startIndex, 0, 0);
        }
    }
}