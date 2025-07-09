#include "EnemySpawnerSystem.h"

void EnemySpawnerSystem::Update(ComponentManager& cm, World& world)
{
    m_elapsedTime += Time::GetDeltaTime();

    View<EnemySpawnMap> view(cm);

    for (auto [entity, spawnMap] : view)
    {
        if (!spawnMap.map.empty())
        {
            if (spawnMap.map.front().spawnTime <= m_elapsedTime)
            {
                const EnemySpawnInfo& info = spawnMap.map.front();

                Entity* spawnedEntity = world.CreateEntity();
                
                // formation ‚É‰ž‚¶‚Äˆ—‚ð•ª‚¯‚é
                EnemySpawnInfo newInfo = info;
                
                if (info.formation != Formation::Boss)
                {
                    world.AddComponent<EnemySpawnInfo>(*spawnedEntity, newInfo);
                }
                else
                {
                    //Entity* boss = world.CreateWithSprite(info.enemySprite.Get(), nullptr, info.position);
                    //world.AddComponent<Boss>(*boss, Boss{});
                    //world.AddComponent<CircleCollider2D>(*boss, CircleCollider2D{ .radius = 100 });
                    //world.AddComponent<Status>(*boss, Status{ .life = 200 });
                }
                
                spawnMap.map.erase(spawnMap.map.begin());
            }
        }
    }
}