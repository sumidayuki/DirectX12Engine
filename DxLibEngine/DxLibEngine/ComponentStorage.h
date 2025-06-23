#pragma once

template <typename T>
class  ComponentStorage : public IComponentStorage
{
private: 
	std::unordered_map<Entity, T> m_datas;

public:
	/// <summary>
	/// 特定のエンティティに指定したコンポーネントを追加します。
	/// </summary>
	/// <param name="entity"></param>
	/// <param name="component"></param>
	void Add(Entity entity, const T& component);

	/// <summary>
	/// 特定のエンティティのコンポーネントを全て削除します。
	/// </summary>
	/// <param name="entity"></param>
	void Remove(Entity entity) override;

	/// <summary>
	/// 特定のエンティティのコンポーネントストレージを取得します。
	/// </summary>
	/// <param name="entity"></param>
	/// <returns></returns>
	T* Get(Entity entity);

	/// <summary>
	/// 全てのコンポーネントストレージを取得します。
	/// </summary>
	/// <returns></returns>
	std::unordered_map<Entity, T>& GetAll() { return m_datas; }
};

template<typename T>
inline void ComponentStorage<T>::Add(Entity entity, const T& component)
{
	m_datas[entity] = component;
}

template<typename T>
inline void ComponentStorage<T>::Remove(Entity entity)
{
	m_datas.erase(entity);
}

template<typename T>
inline T* ComponentStorage<T>::Get(Entity entity)
{
	auto it = m_datas.find(entity);
	return it != m_datas.end() ? &it->second : nullptr;
}
