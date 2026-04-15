#include "MeshRendererSystem.h"
#include "ShaderRegistry.h"
#include "Standard.hlsli"

// 1フレームあたりに描画可能なオブジェクトの最大数
constexpr UINT MAX_OBJECTS_PER_FRAME = 1024;

void MeshRendererSystem::StaticConstructor()
{
}

void MeshRendererSystem::StaticDestructor()
{
    if (m_objectConstantBufferRing && m_mappedObjectConstants)
    {
        m_objectConstantBufferRing->UnlockBufferAfterWrite();
        m_mappedObjectConstants = nullptr;
    }
    m_objectConstantBufferRing.Reset();
}

void MeshRendererSystem::Start(World& world)
{
    // 定数バッファの作成 (256バイトアライメント)
    const UINT alignedSize = (sizeof(ObjectLayout) + 255) & ~255;
    m_objectConstantBufferRing.Attach(new GraphicsBuffer(
        GraphicsBuffer::Target::Constant,
        GraphicsBuffer::UsageFlags::LockBufferForWrite,
        Graphics::BackBafferCount * MAX_OBJECTS_PER_FRAME,
        alignedSize
    ));

    m_mappedObjectConstants = (BYTE*)m_objectConstantBufferRing->LockBufferForWrite();
    m_currentObjectBufferIndex = 0;
}

void MeshRendererSystem::Draw(World& world)
{
    ID3D12GraphicsCommandList* commandList = Graphics::GetCurrentFrameResource()->GetCommandList();

    commandList->SetGraphicsRootSignature(ShaderRegistry::GetRootSignature());

    // BindlessHeapを設定（フレームに1回）
    ID3D12DescriptorHeap* heaps[] = { BindlessHeap::GetInstance()->GetHeap()->GetNativeHeapPointer() };
    commandList->SetDescriptorHeaps(_countof(heaps), heaps);

    CameraSystem* cameraSystem = world.GetSystem<CameraSystem>();
    LightSystem* lightSystem = world.GetSystem<LightSystem>();

    if (!cameraSystem || !lightSystem) return;

    Camera* camera = cameraSystem->GetCurrent();
    if (!camera) return;

    // Slot 0: CameraLayout (b0)
    commandList->SetGraphicsRootConstantBufferView(0, cameraSystem->GetCameraBuffer(*camera)->GetNativeBufferPtr()->GetGPUVirtualAddress());

    // Slot 2: LightConstants (b2)
    commandList->SetGraphicsRoot32BitConstant(2, lightSystem->GetActiveLightCount(), 0);

    // Slot 4: Lights (t0 space0)
    if (lightSystem->GetActiveLightCount() > 0)
    {
        commandList->SetGraphicsRootDescriptorTable(4,
            BindlessHeap::GetInstance()->GetHeap()->GetGPUDescriptorHandle(lightSystem->GetLightSrvIndex())
        );
    }

    // Slot 5: Bindless Textures (t0[] space1)
    commandList->SetGraphicsRootDescriptorTable(5,
        BindlessHeap::GetInstance()->GetHeap()->GetGPUDescriptorHandle(0)
    );

    m_currentObjectBufferIndex = 0;
    const UINT frameIndex = Graphics::GetCurrentFrameResource()->GetFrameIndex();
    const UINT alignedObjectConstantsSize = m_objectConstantBufferRing->GetStride();
    const UINT bufferOffsetForFrame = frameIndex * MAX_OBJECTS_PER_FRAME;
    const D3D12_GPU_VIRTUAL_ADDRESS gpuAddressBase = m_objectConstantBufferRing->GetNativeBufferPtr()->GetGPUVirtualAddress();

    View<Transform, MeshFilter, MeshRenderer> view(world);
    for (auto [entity, transform, meshFilter, renderer] : view)
    {
        if (!meshFilter.mesh) continue;

        GraphicsBuffer* vertexBuffer = meshFilter.mesh->GetVertexBuffer();
        GraphicsBuffer* indexBuffer = meshFilter.mesh->GetIndexBuffer();
        if (!vertexBuffer || !indexBuffer) continue;

        // IA設定
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
            if (m_currentObjectBufferIndex + 1 >= MAX_OBJECTS_PER_FRAME)
            {
                OutputDebugStringA("MeshRenderer Warning: Max objects reached.\n");
                break;
            }

            const auto& submesh = meshFilter.mesh->GetSubMesh(i);
            if (submesh.materialIndex >= renderer.materials.size()) continue;
            Material* material = renderer.materials[submesh.materialIndex];
            if (!material) continue;

            // Object Constants (Slot 1 / b1)
            ObjectLayout constants;
            constants.world = worldMatrix.Transpose();

            BYTE* dest = m_mappedObjectConstants + (bufferOffsetForFrame + m_currentObjectBufferIndex) * alignedObjectConstantsSize;
            memcpy(dest, &constants, sizeof(ObjectLayout));

            D3D12_GPU_VIRTUAL_ADDRESS objGpuAddr = gpuAddressBase + (bufferOffsetForFrame + m_currentObjectBufferIndex) * alignedObjectConstantsSize;
            commandList->SetGraphicsRootConstantBufferView(1, objGpuAddr);
            m_currentObjectBufferIndex++;

            // Material Constants (Slot 3 / b3)
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
                commandList->SetPipelineState(material->GetShader()->GetPSO(false));
                commandList->DrawIndexedInstanced(submesh.indexCount, 1, submesh.startIndex, 0, 0);
            }
        }
    }
}