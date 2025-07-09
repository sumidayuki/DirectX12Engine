#include "EnemyStandardSystem.h"

void EnemyStandardSystem::Start(ComponentManager& cm, World& world)
{
	m_transformSystem = world.GetSystem<TransformSystem>();
}

void EnemyStandardSystem::Update(ComponentManager& cm, World& world)
{
	View<EnemySpawnInfo> view(cm);

	for (auto [entity, info] : view)
	{
		if (info.formation != Formation::Standard) continue;

		info.timer += Time::GetDeltaTime();
		
		if (info.timer >= info.coolDown && info.times > 0)
		{
			Entity* enemy = world.CreateWithSprite(info.texture, Rect(0, 0, 128, 128), Vector2(0.5f, 0.5f), 1.0f, nullptr, info.position, Quaternion(0, 0, 0, 1));
			Transform* transform = world.GetComponent<Transform>(*enemy);
			Vector3 axis = Vector3::forward;
			m_transformSystem->Rotate(*transform, axis, 180);
			world.AddComponent<Enemy>(*enemy, Enemy{ .formation = info.formation });
			world.AddComponent<CircleCollider2D>(*enemy, CircleCollider2D{ .radius = 30 });
			world.AddComponent<Status>(*enemy, Status{ .life = 1 });

			info.times--;
			info.timer = 0;
		}

		if (info.times == 0)
		{
			world.DestoryEntity(entity);
		}
	}
}
