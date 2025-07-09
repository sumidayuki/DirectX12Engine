#include "EnemyWaveSystem.h"

void EnemyWaveSystem::Start(ComponentManager& cm, World& world)
{
	m_transformSystem = world.GetSystem<TransformSystem>();
}

void EnemyWaveSystem::Update(ComponentManager& cm, World& world)
{
	View<EnemySpawnInfo> view(cm);

	for (auto [entity, info] : view)
	{
		if (info.formation != Formation::Wave) continue;

		info.timer += Time::GetDeltaTime();

		if (info.timer > info.coolDown && info.times > 0)
		{
			if (info.right)
			{
				Vector2 vec2 = Vector2(info.position.x, info.position.y);
				info.position = Vector3(vec2 + (Vector2(1, 0) * 150), info.position.z);
				Entity* enemy = world.CreateWithSprite(info.texture, Rect(0, 0, 128, 128), Vector2(0.5f, 0.5f), 1.0f, nullptr, info.position, Quaternion(0, 0, 0, 1));
				world.AddComponent<Enemy>(*enemy, Enemy{ .formation = info.formation });
				world.AddComponent<CircleCollider2D>(*enemy, CircleCollider2D{ .radius = 40 });
				world.AddComponent<Status>(*enemy, Status{ .life = 1 });
			}
			else
			{
				Vector2 vec2 = Vector2(info.position.x, info.position.y);
				info.position = Vector3(vec2 + (Vector2(-1, 0) * 150), info.position.z);				
				Entity* enemy = world.CreateWithSprite(info.texture, Rect(0, 0, 128, 128), Vector2(0.5f, 0.5f), 1.0f, nullptr, info.position, Quaternion(0, 0, 0, 1));
				world.AddComponent<Enemy>(*enemy, Enemy{ .formation = info.formation });
				world.AddComponent<CircleCollider2D>(*enemy, CircleCollider2D{ .radius = 40 });
				world.AddComponent<Status>(*enemy, Status{ .life = 1 });
			}

			info.times--;
			info.timer = 0.0f;
		}
		if (info.times == 0)
		{
			world.DestoryEntity(entity);
		}
	}
}
