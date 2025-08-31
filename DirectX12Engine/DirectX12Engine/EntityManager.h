#pragma once

/// <summary>
/// エンティティを管理します。
/// </summary>
class EntityManager
{
private:
	std::vector<uint32_t> m_generations;	// Entityを格納するキュー
	std::vector<uint32_t> m_freeIds;		// EntityのID

public:
	/// <summary>
	/// エンティティを作成します。
	/// </summary>
	/// <returns></returns>
	Entity* CreateEntity();

	/// <summary>
	/// エンティティを破壊します。
	/// </summary>
	/// <param name="entity">指定したいEntityの値をuint32_t型で取得します。</param>
	void DestroyEntity(Entity* entity);

	bool IsAlive(Entity* entity) const { return entity->id < m_generations.size() && m_generations[entity->id] == entity->generation; }
};