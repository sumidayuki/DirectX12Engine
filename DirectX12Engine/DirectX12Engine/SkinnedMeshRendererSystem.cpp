#include "SkinnedMeshRendererSystem.h"
#include "ShaderRegistry.h"
#include "Standard.hlsli"

// 1フレームあたりに描画可能なオブジェクトの最大数
constexpr UINT MAX_SKINNED_OBJECTS_PER_FRAME = 1024;

D3D12_GPU_DESCRIPTOR_HANDLE SkinnedMeshRendererSystem::GetSRV(Texture2D* tex, DescriptorAllocator* allocator)
{
    if (auto it = m_srvCache.find(tex); it != m_srvCache.end())
    {
        return it->second;
    }

    D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
    desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    desc.Format = tex->GetNativeResource()->GetDesc().Format;
    desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    desc.Texture2D.MipLevels = tex->GetNativeResource()->GetDesc().MipLevels;

    auto handle = allocator->CreateSRV(tex->GetNativeResource(), desc);
    m_srvCache[tex] = handle;
    return handle;
}

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
    // m_graphicsPipelineState.Reset();
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
	m_srvCache.clear();

    ID3D12GraphicsCommandList* commandList = Graphics::GetCurrentFrameResource()->GetCommandList();
    DescriptorAllocator* srvAllocator = world.GetSrvAllocator();

    commandList->SetGraphicsRootSignature(ShaderRegistry::GetRootSignature());

    ID3D12DescriptorHeap* heaps[] = { srvAllocator->GetHeap()->GetNativeHeapPointer() };
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
        commandList->SetGraphicsRootDescriptorTable(4, lightSystem->GetLightBufferGpuHandle());
    }

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
            smr.animator = world.GetComponent<Animator>(TransformSystem::GetInstance()->GetRoot(transform)->entity);
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
            commandList->SetGraphicsRootConstantBufferView(1, objGpuAddr); // Bind to b1 (Param 1)
            m_currentObjectBufferIndex++;


            // --- 3. Material Constants ---
            if (material)
            {
                BYTE* destMat = m_mappedObjectConstants + (bufferOffsetForFrame + m_currentObjectBufferIndex) * alignedObjectConstantsSize;
                // Use generic property buffer
                if (material->GetConstantBufferSize() > 0 && material->GetConstantBufferSize() <= alignedObjectConstantsSize)
                {
                    memcpy(destMat, material->GetConstantBufferData(), material->GetConstantBufferSize());
                }

                D3D12_GPU_VIRTUAL_ADDRESS matGpuAddr = gpuAddressBase + (bufferOffsetForFrame + m_currentObjectBufferIndex) * alignedObjectConstantsSize;
                commandList->SetGraphicsRootConstantBufferView(3, matGpuAddr); // Bind to b3 (Param 3)
                m_currentObjectBufferIndex++;

                const auto& resourceTable = material->GetShader()->GetResourceTable();

                for (auto const& [id, info] : resourceTable)
                {
                    if (info.bindPoint == 0) continue;

                    // 2. マテリアルからテクスチャを取得
                    Texture2D* tex = material->GetTexture(id);

                    // 3. マテリアルに設定されていない場合のみ、デフォルトの白テクスチャを割り当てる
                    if (!tex)
                    {
                        tex = AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/white.png");
                    }

                    // 4. 正しいレジスタ番号に対応するルートパラメータにセット
                    // PBR.hlsl では t1(_MainTex), t2(_NormalTex) ... となっています。
                    // lights(t0) が RootParam 4 なので、tN は 4 + N 番にセットします。
                    if (tex)
                    {
                        D3D12_GPU_DESCRIPTOR_HANDLE handle = GetSRV(tex, srvAllocator);
                        commandList->SetGraphicsRootDescriptorTable(4 + info.bindPoint, handle);
                    }
                }

                if (material->GetShader())
                {
                    commandList->SetPipelineState(material->GetShader()->GetPSO(true));
                    commandList->DrawIndexedInstanced(submesh.indexCount, 1, submesh.startIndex, 0, 0);
                }
            }
        }
    }
}