#include "EnemyCircleSystem.h"

void EnemyCircleSystem::Start(ComponentManager& cm, World& world)
{
	m_transformSystem = world.GetSystem<TransformSystem>();
}

void EnemyCircleSystem::Update(ComponentManager& cm, World& world)
{
	View<EnemySpawnInfo> view(cm);

	for (auto [entity, info] : view)
	{
		if (info.formation != Formation::Circle) continue;

		info.timer += Time::GetDeltaTime();

		if (info.timer >= info.coolDown && info.times > 0)
		{
			Entity* enemy = world.CreateWithSprite(info.texture, Rect(0, 0, 128, 128), Vector2(0.5f, 0.5f), 1.0f, nullptr, info.position, Quaternion(0, 0, 0, 1));
			world.AddComponent<Enemy>(*enemy, Enemy{ .formation = info.formation });
			world.AddComponent<CircleCollider2D>(*enemy, CircleCollider2D{ .radius = 40 });
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
