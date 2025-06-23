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
			Entity enemy = world.CreateEntity();
			Transform* transform = world.GetComponent<Transform>(enemy);
			transform->position = info.position;
			Vector3 axis = Vector3(0, 0, 1);
			m_transformSystem->Rotate(*transform, axis, 180);
			transform->scale = Vector3(0.15f, 0.15f, 0);
			world.AddComponent<Enemy>(enemy, Enemy{ .formation = info.formation });
			world.AddComponent<RenderCommand>(enemy, RenderCommand{.layer = Layer::Enemy, .type = RenderType::Sprite, .sprite = info.enemySprite});
			world.AddComponent<CircleCollider2D>(enemy, CircleCollider2D{ .radius = 40 });
			world.AddComponent<Status>(enemy, Status{ .life = 1 });

			info.times--;
			info.timer = 0;
		}

		if (info.times == 0)
		{
			world.DestoryEntity(entity);
		}
	}
}
