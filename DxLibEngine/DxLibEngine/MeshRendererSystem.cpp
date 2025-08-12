#include "MeshRendererSystem.h"
#include "TransformSystem.h"
#include "CameraSystem.h"
#include "LightSystem.h"

// CHANGED: �V�F�[�_�[�ɓn���萔�o�b�t�@�̃��C�A�E�g���g��
struct MeshRendererSystem::ConstantBufferLayout
{
    Matrix4x4 worldMatrix;
    Color     diffuseColor;
    Color     specularColor;
};

// ���̃V�X�e���ŋ��L����O���t�B�b�N�X���\�[�X�����������܂�
void MeshRendererSystem::StaticConstructor()
{
    ID3D12Device* d3d12Device = Graphics::GetD3D12Device();

    // �V�F�[�_�[�̃R���p�C�� (�ύX�Ȃ�)
    ComPtr<ShaderBytecode> vertexShader;
    vertexShader.Attach(new ShaderBytecode(L"MeshRenderer.hlsl", "VSMain", "vs_5_1"));

    ComPtr<ShaderBytecode> pixelShader;
    pixelShader.Attach(new ShaderBytecode(L"MeshRenderer.hlsl", "PSMain", "ps_5_1"));

    // ���[�g�V�O�l�`��
    D3D12_ROOT_PARAMETER rootParameters[5];
    memset(rootParameters, 0, sizeof(rootParameters));

    // register(b0): �J�����萔�o�b�t�@ (�ύX�Ȃ�)
    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[0].Descriptor.ShaderRegister = 0;
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    // register(b1): �I�u�W�F�N�g�E�}�e���A���萔�o�b�t�@ (�ύX�Ȃ�)
    rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[1].Descriptor.ShaderRegister = 1;
    rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    // �f�B�X�N���v�^�e�[�u��: �����̃e�N�X�`�� (Diffuse, Normal, Specular) ��ݒ�
    D3D12_DESCRIPTOR_RANGE ranges[1];
    memset(ranges, 0, sizeof(ranges));
    ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

    // �����Ńf�X�N���v�^�̐���Material::TextureSlot�̍ő吔�ɐݒ�
    ranges[0].NumDescriptors = (UINT)Material::TextureSlot::Max;
    ranges[0].BaseShaderRegister = 0; // register(t0) ����n�܂�
    ranges[0].RegisterSpace = 0;
    ranges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(ranges);
    rootParameters[2].DescriptorTable.pDescriptorRanges = ranges;
    rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    // ���C�g��� (Structured Buffer) ��ǉ�
    D3D12_DESCRIPTOR_RANGE lightRanges[1];
    memset(lightRanges, 0, sizeof(lightRanges));
    lightRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    lightRanges[0].NumDescriptors = 1;
    lightRanges[0].BaseShaderRegister = 3; // register(t3) �Ƀo�C���h
    lightRanges[0].RegisterSpace = 0;
    lightRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[3].DescriptorTable.NumDescriptorRanges = _countof(lightRanges);
    rootParameters[3].DescriptorTable.pDescriptorRanges = lightRanges;
    rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // �s�N�Z���V�F�[�_�[�Ŏg�p

    // SceneConstants (b2) �������ɒǉ�
    rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[4].Descriptor.ShaderRegister = 2; // b2 �ɑΉ�
    rootParameters[4].Descriptor.RegisterSpace = 0;
    rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // �s�N�Z���V�F�[�_�[�Ŏg�p

    // �ÓI�T���v���[
    D3D12_STATIC_SAMPLER_DESC sampler = {};
    sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    sampler.MaxLOD = D3D12_FLOAT32_MAX;
    sampler.ShaderRegister = 0; // register(s0)
    sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    // ���[�g�V�O�l�`���̃f�X�N���v�^������������
    D3D12_ROOT_SIGNATURE_DESC rsDesc = {};
    rsDesc.NumParameters = _countof(rootParameters);
    rsDesc.pParameters = rootParameters;
    rsDesc.pStaticSamplers = &sampler;
    rsDesc.NumStaticSamplers = 1;
    rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    // ���[�g�V�O�l�`�����V���A���C�Y
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

    // �f�o�C�X���烋�[�g�V�O�l�`�����쐬
    hr = d3d12Device->CreateRootSignature(0, serializedRootSignature->GetBufferPointer(), serializedRootSignature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));
    assert(SUCCEEDED(hr));

    // ���̓��C�A�E�g��TANGENT��ǉ�
    static const D3D12_INPUT_ELEMENT_DESC inputElements[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // TANGENT ��ǉ�
    };

    // �O���t�B�b�N�X�p�C�v���C���X�e�[�g���쐬���܂� (PSO)

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

    // ���X�^���C�U�[�X�e�[�g: ���ʂ��J�����O�i�`�悵�Ȃ��j���܂�
    psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;

    // �[�x�e�X�g��L���ɂ��܂�
    psoDesc.DepthStencilState.DepthEnable = TRUE;
    psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    psoDesc.DepthStencilState.StencilEnable = FALSE;

    // �u�����h�X�e�[�g: ��������L���ɂ��܂�
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
    // StaticConstructor�ō쐬�����ÓI���\�[�X��������܂��B
    // ComPtr�������I��Release()���ĂԂ��߁Anullptr�������邾���ŏ\���ł��B
    m_graphicsPipelineState = nullptr;
    m_rootSignature = nullptr;
    m_defaultWhiteTexture = nullptr;
}

void MeshRendererSystem::Start(ComponentManager& cm, World& world)
{
    TextureImporter importer;

    // TextureImporter���g�킸�Ɏ蓮��Texture2D�I�u�W�F�N�g���쐬
    m_defaultWhiteTexture.Attach(importer.Import(L"Assets/White.png"));

    m_sceneConstantBuffer.Attach(new GraphicsBuffer(
        GraphicsBuffer::Target::Constant,
        GraphicsBuffer::UsageFlags::LockBufferForWrite,
        1,
        sizeof(SceneConstants) // HLSL��cbuffer�̃T�C�Y
    ));
}

void MeshRendererSystem::Draw(ComponentManager& cm, World& world)
{
    ID3D12GraphicsCommandList* commandList = Graphics::GetCurrentFrameResource()->GetCommandList();
    DescriptorAllocator* srvAllocator = world.GetSrvAllocator();

    commandList->SetPipelineState(m_graphicsPipelineState.Get());
    commandList->SetGraphicsRootSignature(m_rootSignature.Get());

    // �ŏ��Ɉ�x�����A���LSRV�q�[�v��ݒ�
    ID3D12DescriptorHeap* heaps[] = { srvAllocator->GetHeap() };
    commandList->SetDescriptorHeaps(_countof(heaps), heaps);

    // ���ʏ��̃o�C���h
    CameraSystem* cameraSystem = world.GetSystem<CameraSystem>();
    Camera* camera = cameraSystem->GetCurrent();
    LightSystem* lightSystem = world.GetSystem<LightSystem>();
    if (!camera || !lightSystem) return;

    // �J����CBV
    commandList->SetGraphicsRootConstantBufferView(0, cameraSystem->GetCameraBuffer(*camera)->GetNativeBufferPtr()->GetGPUVirtualAddress());

    // �V�[��CBV
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

    // ���C�gSRV�e�[�u��
    if (lightSystem->GetActiveLightCount() > 0)
    {
        commandList->SetGraphicsRootDescriptorTable(3, lightSystem->GetLightBufferGpuHandle());
    }

    // �e�I�u�W�F�N�g�̕`�惋�[�v
    View<Transform, MeshRenderer> view(cm);
    for (auto [entity, transform, renderer] : view)
    {
        if (renderer.meshes.empty()) continue;

        const Matrix4x4& worldMatrix = world.GetSystem<TransformSystem>()->GetLocalToWorldMatrix(transform);

        for (const auto& meshComponent : renderer.meshes)
        {
            Mesh* mesh = meshComponent.Get();
            if (!mesh) continue;

            int materialIndex = mesh->GetMaterialIndex();
            if (materialIndex < 0 || materialIndex >= renderer.materials.size()) continue;
            Material* material = renderer.materials[materialIndex].Get();
            if (!material) continue;

            if (!renderer.constantBuffer)
            {
                renderer.constantBuffer.Attach(new GraphicsBuffer(
                    GraphicsBuffer::Target::Constant,
                    GraphicsBuffer::UsageFlags::LockBufferForWrite,
                    1, sizeof(ObjectConstantsLayout)
                ));
            }

            // Object/Material CBV �̍X�V�ƃo�C���h
            ObjectConstantsLayout* lockedPointer = (ObjectConstantsLayout*)renderer.constantBuffer->LockBufferForWrite();
            lockedPointer->worldMatrix = worldMatrix.Transpose();
            lockedPointer->diffuseColor = material->GetDiffuseColor();
            lockedPointer->specularColor = material->GetSpecularColor(); // ���ʔ��ːF��ǉ�
            lockedPointer->shininess = 64.0f; // ����x��ǉ� (�l�͉�)
            renderer.constantBuffer->UnlockBufferAfterWrite();
            commandList->SetGraphicsRootConstantBufferView(1, renderer.constantBuffer->GetNativeBufferPtr()->GetGPUVirtualAddress());

            // Material Textures �̃o�C���h
            if (material->GetGpuDescriptorHandle(Material::TextureSlot::Diffuse).ptr != 0)
            {
                commandList->SetGraphicsRootDescriptorTable(2, material->GetGpuDescriptorHandle(Material::TextureSlot::Diffuse));
            }

            // ���b�V���̃W�I���g������IA�X�e�[�W�ɃZ�b�g
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

            // �`��I
            commandList->DrawIndexedInstanced(mesh->GetIndexCount(), 1, 0, 0, 0);
        }
    }
}