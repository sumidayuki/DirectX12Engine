#pragma once

/// <summary>
/// �G���e�B�e�B���Ǘ����܂��B
/// </summary>
class EntityManager
{
private:
	std::vector<uint32_t> m_generations;	// Entity���i�[����L���[
	std::vector<uint32_t> m_freeIds;		// Entity��ID

public:
	/// <summary>
	/// �G���e�B�e�B���쐬���܂��B
	/// </summary>
	/// <returns></returns>
	Entity* CreateEntity();

	/// <summary>
	/// �G���e�B�e�B��j�󂵂܂��B
	/// </summary>
	/// <param name="entity">�w�肵����Entity�̒l��uint32_t�^�Ŏ擾���܂��B</param>
	void DestroyEntity(Entity* entity);

	bool IsAlive(Entity* entity) const { return entity->id < m_generations.size() && m_generations[entity->id] == entity->generation; }
};