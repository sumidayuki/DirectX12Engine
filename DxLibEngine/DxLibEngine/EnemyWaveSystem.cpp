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
				info.position = info.position + (Vector3::right * 150);

				Entity enemy = world.CreateEntity();
				Transform* transform = world.GetComponent<Transform>(enemy);
				transform->position = info.position;
				Vector3 axis = Vector3(0, 0, 1);
				m_transformSystem->Rotate(*transform, axis, 180);
				transform->scale = Vector3(0.15f, 0.15f, 0);
				world.AddComponent<Enemy>(enemy, Enemy{ .formation = info.formation });
				world.AddComponent<RenderCommand>(enemy, RenderCommand{ .layer = Layer::Enemy, .type = RenderType::Sprite, .sprite = info.enemySprite });
				world.AddComponent<CircleCollider2D>(enemy, CircleCollider2D{ .radius = 40 });
				world.AddComponent<Status>(enemy, Status{ .life = 1 });
			}
			else
			{
				info.position = info.position + (Vector3::left * 150);
				Entity enemy = world.CreateEntity();
				Transform* transform = world.GetComponent<Transform>(enemy);
				transform->position = info.position;
				Vector3 axis = Vector3(0, 0, 1);
				m_transformSystem->Rotate(*transform, axis, 180);
				transform->scale = Vector3(0.15f, 0.15f, 0);
				world.AddComponent<Enemy>(enemy, Enemy{ .formation = info.formation });
				world.AddComponent<RenderCommand>(enemy, RenderCommand{ .layer = Layer::Enemy, .type = RenderType::Sprite, .sprite = info.enemySprite });
				world.AddComponent<CircleCollider2D>(enemy, CircleCollider2D{ .radius = 40 });
				world.AddComponent<Status>(enemy, Status{ .life = 1 });
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
