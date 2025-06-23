#include "EnemySpawnerSystem.h"

void EnemySpawnerSystem::Update(ComponentManager& cm, World& world)
{
    m_elapsedTime += Time::GetDeltaTime();

    View<EnemySpawnMap> view(cm);

    for (auto [entity, esm] : view)
    {
        // esm は EnemySpawnMap のコピーなので、参照として扱う
        auto& spawnMap = cm.GetComponent<EnemySpawnMap>(entity)->map;

        if (!spawnMap.empty() && spawnMap.front().spawnTime <= m_elapsedTime)
        {
            const EnemySpawnInfo& info = spawnMap.front();

            Entity spawnedEntity = world.CreateEntity();
            
            // formation に応じて処理を分ける
            EnemySpawnInfo newInfo = info;
            
            if (info.formation != Formation::Boss)
            {
                world.AddComponent<EnemySpawnInfo>(spawnedEntity, newInfo);
            }
            else
            {
                Entity boss = world.CreateEntity();
                Transform* transform = world.GetComponent<Transform>(boss);
                transform->position = info.position;
                world.AddComponent<Boss>(boss, Boss{});
                world.AddComponent<RenderCommand>(boss, RenderCommand{ .layer = Layer::Enemy, .type = RenderType::Sprite, .sprite = info.enemySprite });
                world.AddComponent<CircleCollider2D>(boss, CircleCollider2D{ .radius = 100 });
                world.AddComponent<Status>(boss, Status{ .life = 200 });
            }

            spawnMap.erase(spawnMap.begin());
        }
    }
}