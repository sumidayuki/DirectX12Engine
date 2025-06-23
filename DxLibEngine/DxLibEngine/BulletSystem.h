#pragma once
#include "TransformSystem.h"

class BulletSystem : public System
{
private:
	TransformSystem* m_transformSystem;

	std::queue<Entity> m_bulletPool;

public:
	void CreateBulletPool(int poolSize, World& world);

	Entity GetBullet(World& world);

	void ReturnBullet(Entity e);

public:
	void Start(ComponentManager& cm, World& world) override;

	void Update(ComponentManager& cm, World& world) override;
};