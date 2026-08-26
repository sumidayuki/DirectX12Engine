#include "SkinnedMeshRendererSystem.h"
#include "ShaderRegistry.h"
#include "Standard.hlsli"

// 1フレームあたりに描画可能なオブジェクトの最大数
constexpr UINT MAX_SKINNED_OBJECTS_PER_FRAME = 1024;

void SkinnedMeshRendererSystem::StaticConstructor()
{
}

void SkinnedMeshRendererSystem::StaticDestructor()
{
    if (m_objectConstantBufferRing && m_mappedObjectConstants)
    {
        m_objectConstantBufferRing->UnlockBufferAfterWrite();
        m_mappedObjectConstants = nullptr;
    }
    m_objectConstantBufferRing.Reset();
}

void SkinnedMeshRendererSystem::Start(World& world)
{
    // 定数バッファの作成
    const UINT alignedSize = (sizeof(SkinnedObjectLayout) + 255) & ~255;
    m_objectConstantBufferRing.Attach(new GraphicsBuffer(
        GraphicsBuffer::Target::Constant,
        GraphicsBuffer::UsageFlags::LockBufferForWrite,
        Graphics::BackBafferCount * MAX_SKINNED_OBJECTS_PER_FRAME,
        alignedSize
    ));
    m_mappedObjectConstants = (BYTE*)m_objectConstantBufferRing->LockBufferForWrite();
    m_currentObjectBufferIndex = 0;
}

void SkinnedMeshRendererSystem::Draw(World& world)
{
    ID3D12GraphicsCommandList* commandList = Graphics::GetCurrentFrameResource()->GetCommandList();

    commandList->SetGraphicsRootSignature(ShaderRegistry::GetRootSignature());

    // BindlessHeapを設定
    ID3D12DescriptorHeap* heaps[] = { BindlessHeap::GetInstance()->GetHeap()->GetNativeHeapPointer() };
    commandList->SetDescriptorHeaps(_countof(heaps), heaps);

    CameraSystem* cameraSystem = world.GetSystem<CameraSystem>();
    LightSystem* lightSystem = world.GetSystem<LightSystem>();

    if (!cameraSystem || !lightSystem) return;

    Camera* camera = cameraSystem->GetCurrent();
    if (!camera) return;

    commandList->SetGraphicsRootConstantBufferView(0, cameraSystem->GetCameraBuffer(*camera)->GetNativeBufferPtr()->GetGPUVirtualAddress());
    commandList->SetGraphicsRoot32BitConstant(2, lightSystem->GetActiveLightCount(), 0);

    if (lightSystem->GetActiveLightCount() > 0)
    {
        commandList->SetGraphicsRootDescriptorTable(4,
            BindlessHeap::GetInstance()->GetHeap()->GetGPUDescriptorHandle(lightSystem->GetLightSrvIndex())
        );
    }

    // Bindless Textures
    commandList->SetGraphicsRootDescriptorTable(5,
        BindlessHeap::GetInstance()->GetHeap()->GetGPUDescriptorHandle(0)
    );

    m_currentObjectBufferIndex = 0;
    const UINT frameIndex = Graphics::GetCurrentFrameResource()->GetFrameIndex();
    const UINT alignedObjectConstantsSize = m_objectConstantBufferRing->GetStride();
    const UINT bufferOffsetForFrame = frameIndex * MAX_SKINNED_OBJECTS_PER_FRAME;
    const D3D12_GPU_VIRTUAL_ADDRESS gpuAddressBase = m_objectConstantBufferRing->GetNativeBufferPtr()->GetGPUVirtualAddress();

    View<SkinnedMeshRenderer, MeshFilter, Transform> view(world);
    for (auto [entity, smr, meshFilter, transform] : view)
    {
        if (!meshFilter.mesh) continue;

        if (m_currentObjectBufferIndex + 1 >= MAX_SKINNED_OBJECTS_PER_FRAME)
        {
            OutputDebugStringA("SkinnedMeshRenderer Warning: Max skinned objects reached.\n");
            break;
        }

        if (!smr.animator)
        {
            smr.animator = world.GetComponent<Animator>(TransformAPI::GetRoot(transform)->entity);
        }
        if (!smr.animator) continue;

        GraphicsBuffer* vertexBuffer = meshFilter.mesh->GetVertexBuffer();
        GraphicsBuffer* indexBuffer = meshFilter.mesh->GetIndexBuffer();
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

        const Matrix4x4& worldMatrix = transform.localToWorldMatrix;

        for (UINT i = 0; i < meshFilter.mesh->GetSubMeshCount(); ++i)
        {
            if (m_currentObjectBufferIndex + 1 >= MAX_SKINNED_OBJECTS_PER_FRAME) break;

            const auto& submesh = meshFilter.mesh->GetSubMesh(i);
            if (submesh.materialIndex >= smr.materials.size()) continue;
            Material* material = smr.materials[submesh.materialIndex];

            SkinnedObjectLayout constants;
            constants.world = worldMatrix.Transpose();
            const size_t boneCount = std::min(smr.animator->skeleton->GetBoneCount(), 256);
            for (size_t j = 0; j < boneCount; ++j)
            {
                constants.boneMatrices[j] = smr.animator->finalBoneMatrices[j].Transpose();
            }

            BYTE* dest = m_mappedObjectConstants + (bufferOffsetForFrame + m_currentObjectBufferIndex) * alignedObjectConstantsSize;
            memcpy(dest, &constants, sizeof(SkinnedObjectLayout));

            D3D12_GPU_VIRTUAL_ADDRESS objGpuAddr = gpuAddressBase + (bufferOffsetForFrame + m_currentObjectBufferIndex) * alignedObjectConstantsSize;
            commandList->SetGraphicsRootConstantBufferView(1, objGpuAddr);
            m_currentObjectBufferIndex++;

            if (material)
            {
                BYTE* destMat = m_mappedObjectConstants + (bufferOffsetForFrame + m_currentObjectBufferIndex) * alignedObjectConstantsSize;
                if (material->GetConstantBufferSize() > 0 && material->GetConstantBufferSize() <= alignedObjectConstantsSize)
                {
                    memcpy(destMat, material->GetConstantBufferData(), material->GetConstantBufferSize());
                }

                D3D12_GPU_VIRTUAL_ADDRESS matGpuAddr = gpuAddressBase + (bufferOffsetForFrame + m_currentObjectBufferIndex) * alignedObjectConstantsSize;
                commandList->SetGraphicsRootConstantBufferView(3, matGpuAddr);
                m_currentObjectBufferIndex++;

                if (material->GetShader())
                {
                    commandList->SetPipelineState(material->GetShader()->GetPSO(true));
                    commandList->DrawIndexedInstanced(submesh.indexCount, 1, submesh.startIndex, 0, 0);
                }
            }
        }
    }
}