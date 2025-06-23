#include "World.h"

Entity World::CreateEntity()
{
	// Entity �� entityManager ���g�p���ăG���e�B�e�B�𐶐����܂��B
	Entity entity = m_em.CreateEntity();

	// Entity �� Transform �R���|�[�l���g��t���Ă����܂��B
	m_cm.AddComponent<Transform>(entity, Transform{});

	return entity;
}

void World::DestoryEntity(Entity entity)
{
	m_cm.RemoveAllComponents(entity);
	m_em.DestroyEntity(entity);
}

void World::AddSystem(std::unique_ptr<System> sys)
{
	m_systems.push_back(std::move(sys));
}

void World::Start(World& world)
{
	for (auto& sys : m_systems)
	{
		sys->Start(m_cm, world);
	}
}

void World::Update(World& world)
{
	for (auto& sys : m_systems)
	{
		sys->Update(m_cm, world);
	}
}

void World::Draw(World& world)
{
	for (auto& sys : m_systems)
	{
		sys->Draw(m_cm, world);
	}
}
