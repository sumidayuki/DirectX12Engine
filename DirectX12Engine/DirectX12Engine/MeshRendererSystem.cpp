#include "MeshRendererSystem.h"
#include "TransformSystem.h"
#include "CameraSystem.h"
#include "LightSystem.h"
#include "ShaderRegistry.h"
#include "Standard.hlsli"

// 1フレームあたりに描画可能なオブジェクトの最大数
constexpr UINT MAX_OBJECTS_PER_FRAME = 2048;

// このシステムで共有するグラフィックスリソースを初期化します
void MeshRendererSystem::StaticConstructor()
{
}

void MeshRendererSystem::StaticDestructor()
{
    // リングバッファのマップを解除
    if (m_objectConstantBufferRing && m_mappedObjectConstants)
    {
        m_objectConstantBufferRing->UnlockBufferAfterWrite();
        m_mappedObjectConstants = nullptr;
    }

    m_defaultWhiteTexture = nullptr;
    m_objectConstantBufferRing = nullptr;
}

D3D12_GPU_DESCRIPTOR_HANDLE MeshRendererSystem::GetSRV(Texture2D* tex, DescriptorAllocator* allocator)
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

void MeshRendererSystem::Start(World& world)
{
    TextureImporter importer;
    m_defaultWhiteTexture.Attach(importer.Import(L"Assets/White.png"));

    // リングバッファの初期化
    // D3D12の定数バッファの要件である256バイトアライメントを計算
    const UINT alignedObjectConstantsSize = (sizeof(ObjectLayout) + 255) & ~255;

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

void MeshRendererSystem::Draw(World& world)
{
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

    // 0: CameraLayout (b0)
    commandList->SetGraphicsRootConstantBufferView(0, cameraSystem->GetCameraBuffer(*camera)->GetNativeBufferPtr()->GetGPUVirtualAddress());

    // 2: LightConstants (b2) - ライト数など
    commandList->SetGraphicsRoot32BitConstant(2, lightSystem->GetActiveLightCount(), 0);

    // オブジェクト用リングバッファのセットアップ
    m_currentObjectBufferIndex = 0;

    const UINT frameIndex = Graphics::GetCurrentFrameResource()->GetFrameIndex();
    const UINT alignedSize = m_objectConstantBufferRing->GetStride();
    const UINT frameOffset = frameIndex * MAX_OBJECTS_PER_FRAME;
    const D3D12_GPU_VIRTUAL_ADDRESS gpuAddressBase = m_objectConstantBufferRing->GetNativeBufferPtr()->GetGPUVirtualAddress();

    // 描画対象のエンティティを取得
    View<Transform, MeshFilter, MeshRenderer> view(world);
    for (auto [entity, transform, meshFilter, renderer] : view)
    {
        if (!meshFilter.mesh) continue;
        Mesh* mesh = meshFilter.mesh;
        if (!mesh) continue;

        // 入力アセンブラ設定（IAステージ）
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

        // サブメッシュごとの描画処理
        const Matrix4x4& worldMatrix = world.GetSystem<TransformSystem>()->GetLocalToWorldMatrix(transform);

        for (UINT i = 0; i < mesh->GetSubMeshCount(); ++i)
        {
            if (m_currentObjectBufferIndex >= MAX_OBJECTS_PER_FRAME)
            {
                OutputDebugStringA("Warning: Maximum drawable object count reached.\n");
                break;
            }

            const SubMesh& subMesh = mesh->GetSubMesh(i);
            if (subMesh.materialIndex >= renderer.materials.size()) continue;

            Material* material = renderer.materials[subMesh.materialIndex];
            if (!material) continue;

            ObjectLayout layout;
            layout.world = worldMatrix.Transpose();

            BYTE* dest = m_mappedObjectConstants + (frameOffset + m_currentObjectBufferIndex) * alignedSize;
            memcpy(dest, &layout, sizeof(ObjectLayout));

            D3D12_GPU_VIRTUAL_ADDRESS objGpuAddr = gpuAddressBase + (frameOffset + m_currentObjectBufferIndex) * alignedSize;
            commandList->SetGraphicsRootConstantBufferView(1, objGpuAddr);

            m_currentObjectBufferIndex++;

            BYTE* destMat = m_mappedObjectConstants + (frameOffset + m_currentObjectBufferIndex) * alignedSize;
            memcpy(destMat, material->GetConstantBufferData(), material->GetConstantBufferSize());

            D3D12_GPU_VIRTUAL_ADDRESS matGpuAddr = gpuAddressBase + (frameOffset + m_currentObjectBufferIndex) * alignedSize;
            commandList->SetGraphicsRootConstantBufferView(3, matGpuAddr);
            m_currentObjectBufferIndex++;

            // t0: ライト構造化バッファ
            commandList->SetGraphicsRootDescriptorTable(4, lightSystem->GetLightBufferGpuHandle());

            int texSlot = 0;
            for (auto const& [id, texture] : material->GetTextures()) 
            {
                Texture2D* tex = material->GetTexture(id);

                // マテリアルに設定されていない場合は、デフォルトの白テクスチャを使う
                if (!tex) tex = m_defaultWhiteTexture.Get();

                if (tex) 
                {
                    D3D12_GPU_DESCRIPTOR_HANDLE handle = GetSRV(tex, srvAllocator);
                    commandList->SetGraphicsRootDescriptorTable(5 + texSlot, handle);
					texSlot++;
                }
            }

            // 描画コマンド発行
            if (material && material->GetShader())
            {
                commandList->SetPipelineState(material->GetShader()->GetPSO(false));
                commandList->DrawIndexedInstanced(subMesh.indexCount, 1, subMesh.startIndex, 0, 0);
            }
        }
    }
}