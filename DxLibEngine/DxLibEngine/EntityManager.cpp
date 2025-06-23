#include "EntityManager.h"

Entity EntityManager::CreateEntity()
{
    uint32_t id;

    if (!m_freeIds.empty())
    {
        id = m_freeIds.back();
        m_freeIds.pop_back();
    }
    else
    {
        id = m_generations.size();
        m_generations.push_back(0);
    }

    return Entity{ id, m_generations[id] };
}

void EntityManager::DestroyEntity(Entity entity)
{
    if (!IsAlive(entity)) return;
    m_generations[entity.id]++;
    m_freeIds.push_back(entity.id);
}