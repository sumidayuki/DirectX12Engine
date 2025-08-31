#include "LightSystem.h"

void LightSystem::Start(ComponentManager& cm, World& world)
{
    // ���C�g�����i�[���邽�߂�GraphicsBuffer���쐬
    m_lightBuffer.Attach(new GraphicsBuffer(
        GraphicsBuffer::Target::Structured,
        GraphicsBuffer::UsageFlags::LockBufferForWrite,
        MAX_LIGHT,
        sizeof(Light)
    ));

    DescriptorAllocator* allocator = world.GetSrvAllocator();

    // SRV�̍쐬
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
    // �A�N�e�B�u�ȃ��C�g�̃��X�g���ꎞ�I�ɍ쐬
    static std::vector<Light> activeLights;
    activeLights.clear();

    View<Transform, Light> view(cm);
    TransformSystem* transformSystem = world.GetSystem<TransformSystem>();

    // �V�[�����̑S�Ẵ��C�g�𑖍�
    for (auto [entity, transform, light] : view)
    {
        if (light.enabled)
        {
            // Transform�R���|�[�l���g���烏�[���h���W�ƌ������v�Z���A
            // Light�\���̂ɐݒ肷��
            light.position = transform.position;
            light.direction = Vector3(transform.rotation.x, transform.rotation.y, transform.rotation.z);
            activeLights.push_back(light);
        }

        // ���C�g�̍ő吔�ɒB�����烋�[�v�𔲂���
        if (activeLights.size() >= MAX_LIGHT)
        {
            break;
        }
    }

    m_activeLightCount = (int)activeLights.size();

    // ���C�g���1�ȏ゠��΁A�o�b�t�@�ɏ�������
    if (m_activeLightCount > 0)
    {
        Light* lockedPointer = (Light*)m_lightBuffer->LockBufferForWrite(0, m_activeLightCount);
        memcpy(lockedPointer, activeLights.data(), m_activeLightCount * sizeof(Light));
        m_lightBuffer->UnlockBufferAfterWrite();
    }
}