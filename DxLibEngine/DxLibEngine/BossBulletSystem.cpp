#include "BossBulletSystem.h"

void BossBulletSystem::Start(ComponentManager& cm, World& world)
{
	m_transformSystem = world.GetSystem<TransformSystem>();
}

void BossBulletSystem::Update(ComponentManager& cm, World& world)
{
	View<Transform, BossBullet, Velocity, CircleCollider2D> view(cm);

	for (auto [entity, transform, bullet, vel, coll] : view)
	{
		if (!entity.enabled) continue;

		if (bullet.bulletType == BulletType::normal)
		{
			Vector3 forward
			(
				transform.localToWorldMatrix._21,  // YŽ²‚ÌX¬•ª
				transform.localToWorldMatrix._22,  // YŽ²‚ÌY¬•ª
				0
			);

			m_transformSystem->Translate(transform, forward * vel.speed * Time::GetDeltaTime());

			if (transform.position.y < 0 || transform.position.y > Screen::GetHeight() || transform.position.x < 0 || transform.position.x > Screen::GetWidth())
			{
				world.DestoryEntity(entity);
				continue;
			}
		}
		else if (bullet.bulletType == BulletType::laser)
		{
			static float timer = 0;

			timer += Time::GetDeltaTime();

			if (timer > 3)
			{
				world.DestoryEntity(entity);
				continue;
			}
		}

		if (coll.info.state == CollisionState::Enter || coll.info.state == CollisionState::Stay)
		{
			if (world.HasComponent<Player>(coll.info.other) && world.IsAlive(coll.info.other))
			{
				world.GetComponent<Status>(coll.info.other)->life -= bullet.damage;

				world.DestoryEntity(entity);
			}
		}
	}
}
