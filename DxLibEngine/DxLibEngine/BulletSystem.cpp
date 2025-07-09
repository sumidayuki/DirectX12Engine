#include "BulletSystem.h"

void BulletSystem::CreateBulletPool(int poolSize, World& world)
{
	for (int i = 0; i < poolSize; i++)
	{
		Entity* e = world.CreateEntity();
	}
}

Entity BulletSystem::GetBullet(World& world)
{
	return Entity();
}

void BulletSystem::ReturnBullet(Entity e)
{
}

void BulletSystem::Start(ComponentManager& cm, World& world)
{
	m_transformSystem = world.GetSystem<TransformSystem>();
}

void BulletSystem::Update(ComponentManager& cm, World& world)
{
	View<Bullet, Velocity, Transform, CircleCollider2D> view(cm);

	for (auto [entity, bullet, velocity, transform, circleCollider] : view)
	{
		if (!world.IsAlive(entity)) continue;

		Vector3 forward
		(
			transform.localToWorldMatrix._21,  // YŽ²‚ÌX¬•ª
			transform.localToWorldMatrix._22,  // YŽ²‚ÌY¬•ª
			0
		);

		m_transformSystem->Translate(transform, forward * velocity.speed * Time::GetDeltaTime());

		if (transform.position.y > 1920 || transform.position.x > 1920 || transform.position.x > Screen::GetWidth())
		{
			world.DestoryEntity(entity);
			continue;
		}

		if (circleCollider.info.state == CollisionState::Enter || circleCollider.info.state == CollisionState::Stay)
		{
			if (world.HasComponent<Enemy>(circleCollider.info.other) || world.HasComponent<Boss>(circleCollider.info.other))
			{
				if (!world.IsAlive(circleCollider.info.other))continue;
			 	Status* status = world.GetComponent<Status>(circleCollider.info.other);
				status->life -= bullet.damage;
				status->isDamaged = true;
				world.DestoryEntity(entity);
			}
		}
	}
}
