#include "ComponentManager.h"

// 修正：静的メンバー変数の定義と初期化を追加
std::unordered_map<std::type_index, size_t> ComponentManager::s_componentTypeMap;
size_t ComponentManager::s_componentTypeIndex = 0;

size_t ComponentManager::GetComponentTypeIndex(std::type_index type)
{
	auto it = s_componentTypeMap.find(type);
	if (it != s_componentTypeMap.end())
	{
		return it->second;
	}
	return static_cast<size_t>(-1);
}

void ComponentManager::RemoveAllComponents(Entity entity)
{
	auto it = m_entitySignatures.find(entity);
	if (it == m_entitySignatures.end()) return;

	for (auto& [type, storage] : m_storages)
	{
		size_t typeIndex = GetComponentTypeIndex(type);
		if (typeIndex != static_cast<size_t>(-1) && typeIndex < it->second.size() && it->second[typeIndex] == 1)
		{
			storage->Remove(entity);
		}
	}
	m_entitySignatures.erase(it);
}