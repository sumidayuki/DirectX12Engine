#include "LightSystem.h"

D3D12_GPU_DESCRIPTOR_HANDLE LightSystem::GetLightBufferGpuHandle() const
{
	return m_gpuDescriptorHandle;
}

int LightSystem::GetActiveLightCount() const
{
	return 0;
}

void LightSystem::Start(ComponentManager& cm, World& world)
{
	// ƒ‰ƒCƒgî•ñ‚ğŠi”[‚·‚é‚½‚ß‚ÌGraphicsBuffer‚ğì¬
	m_lightBuffer.Attach(new GraphicsBuffer
	(
		GraphicsBuffer::Target::Structured,
		GraphicsBuffer::UsageFlags::LockBufferForWrite,
		MAX_LIGHT,
		sizeof(Light)
	));
}

void LightSystem::Update(ComponentManager& cm, World& world)
{
	View<Transform, Light> view(cm);

	for (auto [entity, transform, light] : view)
	{
		
	}
}