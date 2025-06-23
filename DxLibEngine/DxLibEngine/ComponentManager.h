#pragma once

class ComponentManager
{
private:
	std::unordered_map<std::type_index, std::unique_ptr<IComponentStorage>> m_storages;

public:

	template <typename T> ComponentStorage<T>* GetStorage();

	/// <summary>
	/// 特定のエンティティに指定のコンポーネントを追加します。
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="entity"></param>
	/// <param name="component"></param>
	template <typename T> void AddComponent(Entity entity, const T& component);

	template <typename T> bool HasComponent(Entity entity);

	template <typename... Ts> bool HasComponents(Entity entity);

	/// <summary>
	/// 特定のエンティティのコンポーネントを削除します。
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="entity"></param>
	template <typename T> void RemoveComponent(Entity entity);

	void RemoveAllComponents(Entity entity);

	/// <summary>
	/// 特定のエンティティから指定のコンポーネントを取得します。
	/// </summary>
	/// <typeparam name="T">取得したいコンポーネント</typeparam>
	/// <param name="entity"></param>
	/// <returns></returns>
	template <typename T> T* GetComponent(Entity entity) { return GetStorage<T>()->Get(entity); }
	
	/// <summary>
	/// 全てのコンポーネントを取得します。
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <returns></returns>
	template <typename T> std::unordered_map<Entity, T>& GetAllComponents() { return GetStorage<T>()->GetAll(); }
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
inline void ComponentManager::AddComponent(Entity entity, const T& component)
{
	GetStorage<T>()->Add(entity, component);
}

template<typename T>
inline bool ComponentManager::HasComponent(Entity entity)
{
	auto* storage = GetStorage<T>();
	return storage->GetAll().contains(entity);
}

template<typename ...Ts>
inline bool ComponentManager::HasComponents(Entity entity)
{
	return (HasComponent<Ts>(entity) && ...);
}

template<typename T>
inline void ComponentManager::RemoveComponent(Entity entity)
{
	GetStorage<T>()->Remove(entity);
}
