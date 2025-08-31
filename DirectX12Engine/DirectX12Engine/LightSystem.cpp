#include "LightSystem.h"

void LightSystem::Start(ComponentManager& cm, World& world)
{
    // ライト情報を格納するためのGraphicsBufferを作成
    m_lightBuffer.Attach(new GraphicsBuffer(
        GraphicsBuffer::Target::Structured,
        GraphicsBuffer::UsageFlags::LockBufferForWrite,
        MAX_LIGHT,
        sizeof(Light)
    ));

    DescriptorAllocator* allocator = world.GetSrvAllocator();

    // SRVの作成
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Buffer.FirstElement = 0;
    srvDesc.Buffer.NumElements = MAX_LIGHT;
    srvDesc.Buffer.StructureByteStride = sizeof(Light);
    srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

    m_lightBufferGpuHandle = allocator->CreateSrv(m_lightBuffer->GetNativeBufferPtr(), srvDesc);
}

void LightSystem::Update(ComponentManager& cm, World& world)
{
    // アクティブなライトのリストを一時的に作成
    static std::vector<Light> activeLights;
    activeLights.clear();

    View<Transform, Light> view(cm);
    TransformSystem* transformSystem = world.GetSystem<TransformSystem>();

    // シーン内の全てのライトを走査
    for (auto [entity, transform, light] : view)
    {
        if (light.enabled)
        {
            // Transformコンポーネントからワールド座標と向きを計算し、
            // Light構造体に設定する
            light.position = transform.position;
            light.direction = Vector3(transform.rotation.x, transform.rotation.y, transform.rotation.z);
            activeLights.push_back(light);
        }

        // ライトの最大数に達したらループを抜ける
        if (activeLights.size() >= MAX_LIGHT)
        {
            break;
        }
    }

    m_activeLightCount = (int)activeLights.size();

    // ライト情報が1つ以上あれば、バッファに書き込む
    if (m_activeLightCount > 0)
    {
        Light* lockedPointer = (Light*)m_lightBuffer->LockBufferForWrite(0, m_activeLightCount);
        memcpy(lockedPointer, activeLights.data(), m_activeLightCount * sizeof(Light));
        m_lightBuffer->UnlockBufferAfterWrite();
    }
}