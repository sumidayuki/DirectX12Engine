#pragma once

template <typename T>
class  ComponentStorage : public IComponentStorage
{
private: 
	std::unordered_map<Entity, T> m_datas;

public:
	/// <summary>
	/// ����̃G���e�B�e�B�Ɏw�肵���R���|�[�l���g��ǉ����܂��B
	/// </summary>
	/// <param name="entity"></param>
	/// <param name="component"></param>
	void Add(Entity entity, const T& component);

	/// <summary>
	/// ����̃G���e�B�e�B�̃R���|�[�l���g��S�č폜���܂��B
	/// </summary>
	/// <param name="entity"></param>
	void Remove(Entity entity) override;

	/// <summary>
	/// ����̃G���e�B�e�B�̃R���|�[�l���g�X�g���[�W���擾���܂��B
	/// </summary>
	/// <param name="entity"></param>
	/// <returns></returns>
	T* Get(Entity entity);

	/// <summary>
	/// �S�ẴR���|�[�l���g�X�g���[�W���擾���܂��B
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
