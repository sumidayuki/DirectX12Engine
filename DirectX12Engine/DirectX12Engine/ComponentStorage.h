#pragma once

#include "IComponentStorage.h"
#include <vector>
#include <unordered_map>

template <typename T>
class ComponentStorage : public IComponentStorage
{
private:
	std::deque<T> m_datas;
	std::unordered_map<Entity, size_t> m_entityToIndex;
	std::deque<Entity> m_indexToEntity;

public:
	void Add(Entity entity, const T& component)
	{
		size_t newIndex = m_datas.size();
		m_datas.push_back(component);
		m_entityToIndex[entity] = newIndex;
		m_indexToEntity.push_back(entity);
	}

	void Remove(Entity entity) override
	{
		auto it = m_entityToIndex.find(entity);
		if (it == m_entityToIndex.end()) return;

		size_t indexToRemove = it->second;
		size_t lastIndex = m_datas.size() - 1;

		if (indexToRemove != lastIndex)
		{
			m_datas[indexToRemove] = std::move(m_datas.back());
			Entity entityOfLastElement = m_indexToEntity.back();
			m_entityToIndex[entityOfLastElement] = indexToRemove;
			m_indexToEntity[indexToRemove] = entityOfLastElement;
		}

		m_datas.pop_back();
		m_entityToIndex.erase(it);
		m_indexToEntity.pop_back();
	}

	T* Get(Entity entity)
	{
		auto it = m_entityToIndex.find(entity);
		if (it == m_entityToIndex.end()) return nullptr;
		return &m_datas[it->second];
	}

	const std::deque<T>& GetAll() const { return m_datas; }
	const std::deque<Entity>& GetEntities() const { return m_indexToEntity; }
};