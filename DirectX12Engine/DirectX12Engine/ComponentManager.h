#pragma once

#include "IComponentStorage.h"
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <tuple>
#include "ComponentStorage.h"

using Signature = std::vector<size_t>;

class ComponentManager
{
private:
    std::unordered_map<std::type_index, std::unique_ptr<IComponentStorage>> m_storages;
    std::unordered_map<Entity, Signature> m_entitySignatures;
    static std::unordered_map<std::type_index, size_t> s_componentTypeMap;
    static size_t s_componentTypeIndex;

public:
    template <typename T> ComponentStorage<T>* GetStorage();

    static size_t GetComponentTypeIndex(std::type_index type);
    template <typename T> static size_t GetComponentTypeIndex();

    template <typename T> void AddComponent(Entity entity, const T& component)
    {
        GetStorage<T>()->Add(entity, component);
        size_t typeIndex = GetComponentTypeIndex<T>();
        if (m_entitySignatures.find(entity) == m_entitySignatures.end()) {
            m_entitySignatures[entity].resize(s_componentTypeIndex, 0);
        }
        if (typeIndex >= m_entitySignatures[entity].size()) {
            m_entitySignatures[entity].resize(typeIndex + 1, 0);
        }
        m_entitySignatures[entity][typeIndex] = 1;
    }

    template <typename T> bool HasComponent(Entity entity)
    {
        size_t typeIndex = GetComponentTypeIndex<T>();
        if (m_entitySignatures.count(entity) == 0 || typeIndex >= m_entitySignatures[entity].size())
        {
            return false;
        }
        return m_entitySignatures[entity][typeIndex] == 1;
    }

    template <typename ...Ts> bool HasComponents(Entity entity)
    {
        return (HasComponent<Ts>(entity) && ...);
    }

    template <typename T> void RemoveComponent(Entity entity)
    {
        GetStorage<T>()->Remove(entity);
        size_t typeIndex = GetComponentTypeIndex<T>();
        if (m_entitySignatures.count(entity) > 0 && typeIndex < m_entitySignatures[entity].size())
        {
            m_entitySignatures[entity][typeIndex] = 0;
        }
    }

    void RemoveAllComponents(Entity entity);

    template <typename T> T* GetComponent(Entity entity)
    {
        return GetStorage<T>()->Get(entity);
    }

    template <typename T>
    const std::deque<Entity>& GetEntities()
    {
        return GetStorage<T>()->GetEntities();
    }
};

template<typename T>
inline ComponentStorage<T>* ComponentManager::GetStorage()
{
    std::type_index type = typeid(T);
    if (m_storages.find(type) == m_storages.end())
    {
        m_storages[type] = std::make_unique<ComponentStorage<T>>();
    }
    return static_cast<ComponentStorage<T>*>(m_storages[type].get());
}

template<typename T>
inline size_t ComponentManager::GetComponentTypeIndex()
{
    std::type_index type = typeid(T);
    if (s_componentTypeMap.find(type) == s_componentTypeMap.end())
    {
        s_componentTypeMap[type] = s_componentTypeIndex++;
    }
    return s_componentTypeMap[type];
}