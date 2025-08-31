#include "ComponentManager.h"

void ComponentManager::RemoveAllComponents(Entity entity)
{
	for (auto& [type, storage] : m_storages)
	{
		storage->Remove(entity);
	}
}
