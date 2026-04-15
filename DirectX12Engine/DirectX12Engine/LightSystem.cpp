#include "LightSystem.h"
#include "Light.hlsli"
#include "Graphics.h"
#include "World.h"
#include "TransformSystem.h"

void LightSystem::Start(World& world)
{
    // 構造化バッファの作成
    m_lightBuffer.Attach(new GraphicsBuffer(
        GraphicsBuffer::Target::Structured,
        GraphicsBuffer::UsageFlags::LockBufferForWrite,
        MAX_LIGHT,
        sizeof(LightLayout)
    ));
    m_activeLightCount = 0;

    // BindlessHeapにLightバッファSRVを登録（1回だけ）
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Buffer.FirstElement = 0;
    srvDesc.Buffer.NumElements = MAX_LIGHT;
    srvDesc.Buffer.StructureByteStride = sizeof(LightLayout);

    m_lightSrvIndex = BindlessHeap::GetInstance()->RegisterBuffer(
        m_lightBuffer->GetNativeBufferPtr(), srvDesc);
}

void LightSystem::Update(World& world)
{
    static std::vector<LightLayout> activeLightsLayout;
    activeLightsLayout.clear();

    View<Transform, Light> view(world);
    for (auto [entity, transform, light] : view)
    {
        if (transform.isActive)
        {
            LightLayout layout;
            layout.color = light.color;
            layout.position = transform.position;
            layout.range = light.range;
            layout.direction = (transform.rotation * Vector3::forward).Normalized();
            layout.spotAngle = light.spotAngle;
            layout.type = (uint32_t)light.type;
            activeLightsLayout.push_back(layout);
        }
        if (activeLightsLayout.size() >= MAX_LIGHT) break;
    }

    m_activeLightCount = (int)activeLightsLayout.size();

    // GPUバッファへデータをコピー
    if (m_activeLightCount > 0)
    {
        void* pData = m_lightBuffer->LockBufferForWrite(0, m_activeLightCount);
        memcpy(pData, activeLightsLayout.data(), sizeof(LightLayout) * m_activeLightCount);
        m_lightBuffer->UnlockBufferAfterWrite();
    }
}