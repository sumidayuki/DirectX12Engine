#include "DebugSystem.h"

void DebugSystem::Start(World& world)
{
	m_isActive = true;
}

void DebugSystem::Update(World& world)
{
#ifdef _DEBUG
	if (Keyboard::GetKeyState(KeyCode::F1).IsPressed())
	{
		m_isActive = !m_isActive;
	}

	View<DebugRenderer> view(world);
	for (auto [entity, debug] : view)
	{
		if (!m_isActive)
		{
			world.DestroyEntity(debug.debugEntity);
			debug.debugEntity = INVALID_ENTITY;
		}
	}
#endif
}