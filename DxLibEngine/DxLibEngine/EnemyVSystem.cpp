#include "EnemyVSystem.h"

void EnemyVSystem::Start(ComponentManager& cm, World& world)
{
	m_transformSystem = world.GetSystem<TransformSystem>();
}

void EnemyVSystem::Update(ComponentManager& cm, World& world)
{
	View<EnemySpawnInfo> view(cm);

	for (auto [entity, info] : view)
	{
        // �t�H�[���[�V������V�łȂ��Ȃ炱��for���𔲂��܂��B
		if (info.formation != Formation::V) continue;

        info.timer += Time::GetDeltaTime();

        // �^�C�}�[���N�[���_�E���𒴂��Ă��邩������������̐������𒴂��Ă��Ȃ���
        if (info.timer >= info.coolDown && info.times > 0)
        {
            // ���E��2�̂��o��
            for (int i = 0; i < 2; ++i)
            {
                int dir = (i == 0) ? -1 : 1; // �� or �E
                Vector3 offset = Vector3(dir * 75 * info.times, info.position.y - (100 * info.times), 0);
                Vector3 spawnPos = info.position + offset;

				Entity enemy = world.CreateEntity();
				Transform* transform = world.GetComponent<Transform>(enemy);
				transform->position = spawnPos;
				Vector3 axis = Vector3(0, 0, 1);
				m_transformSystem->Rotate(*transform, axis, 180);
				transform->scale = Vector3(0.15f, 0.15f, 0);
				world.AddComponent<Enemy>(enemy, Enemy{ .formation = info.formation });
				world.AddComponent<RenderCommand>(enemy, RenderCommand{ .layer = Layer::Enemy, .type = RenderType::Sprite, .sprite = info.enemySprite });
				world.AddComponent<CircleCollider2D>(enemy, CircleCollider2D{ .radius = 30 });
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
