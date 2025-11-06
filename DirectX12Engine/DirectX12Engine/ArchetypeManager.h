#pragma once
#include "Archetype.h"
#include "Chunk.h"
#include "TypeInfo.h"
#include "IComponentData.h"

class ArchetypeManager
{
private:
	std::vector<std::unique_ptr<Archetype>> m_archetypes;
	std::unordered_map<uint64_t, std::vector<std::unique_ptr<Chunk>>> m_chunks;
	std::unordered_map<Entity, Chunk*> m_entityToChunk;
	std::unordered_map<Entity, const Archetype*> m_entityToArchetype;

public:
	ArchetypeManager() = default;

	/// <summary>
	/// 新しいArchetypeを登録または既存のものを取得します。
	/// </summary>
	/// <param name="typeList"></param>
	/// <returns></returns>
	const Archetype* GetOrCreateArchetype(std::vector<TypeInfo> typeList);

	/// <summary>
	/// 空きのあるChunkを取得します。なければ新規作成します。
	/// </summary>
	/// <param name="archetype"></param>
	/// <returns></returns>
	Chunk* GetOrCreateChunk(const Archetype* archetype);

	/// <summary>
	/// Entityを新規追加します。(既にArchetype構成が決まっている際に有効です。)
	/// </summary>
	/// <param name="archetype"></param>
	/// <param name="entity"></param>
	/// <returns></returns>
	Entity AddEntity(const Archetype* archetype, Entity entity);

	template<ComponentType T>
	T* AddComponent(Entity entity, const T& value);

	template<ComponentType T>
	T* GetComponent(Entity entity);

	void DestroyEntity(Entity entity);

private:
	/// <summary>
	/// Entityを新しいArchetypeに移動します。
	/// 旧Chunkから削除し、新Chunkにコピーします。
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="entity"></param>
	/// <param name="oldArch"></param>
	/// <param name="newArch"></param>
	/// <param name="value"></param>
	template<ComponentType T>
	void MoveEntityToNewArchetype(Entity entity, const Archetype* oldArch, const Archetype* newArch, const T& value);

	/// <summary>
	/// Archetypeをハッシュ化します。
	/// </summary>
	/// <param name="archetype"></param>
	/// <returns></returns>
	uint64_t HashArchetype(const Archetype& archetype) const noexcept;
};

template<ComponentType T>
inline T* ArchetypeManager::AddComponent(Entity entity, const T& value)
{
	// 現在のArchetypeを取得
	const Archetype* oldArch = m_entityToArchetype[entity];
	std::vector<TypeInfo> newTypes = oldArch->types;
	newTypes.push_back(GetTypeInfo<T>());

	const Archetype* newArch = GetOrCreateArchetype(std::move(newTypes));

	// Archetypeが同じなら単純追加
	if (oldArch == newArch)
	{
		Chunk* chunk = m_entityToChunk[entity];
		return chunk->AddComponent<T>(entity, value);
	}

	// 違う場合はChunk間移動を行う
	MoveEntityToNewArchetype<T>(entity, oldArch, newArch, value);
	return GetComponent<T>(entity);
}

template<ComponentType T>
inline T* ArchetypeManager::GetComponent(Entity entity)
{
	auto it = m_entityToChunk.find(entity);
	if (it == m_entityToChunk.end())
	{
		return nullptr;
	}

	return it->second->GetComponent<T>(entity);
}

template<ComponentType T>
inline void ArchetypeManager::MoveEntityToNewArchetype(Entity entity, const Archetype* oldArch, const Archetype* newArch, const T& value)
{
	Chunk* oldChunk = m_entityToChunk[entity];
	size_t oldIndex = oldChunk->FindEntityIndex(entity);
	if (oldIndex == SIZE_MAX) return; // 安全確認

	// 新チャンク確保（空きがなければ作られる）
	Chunk* newChunk = GetOrCreateChunk(newArch);
	size_t newIndex = newChunk->AddEntity(entity);

	// oldArch の全コンポーネントをコピー
	for (const auto& type : oldArch->types)
	{
		if (type.hash == GetTypeInfo<T>().hash) continue;

		const void* src = oldChunk->ReadComponentByHash(oldIndex, type.hash);
		if (src)
		{
			newChunk->WriteComponentByHash(newIndex, type.hash, src);
		}
	}

	// 新しいコンポーネントを書き込む
	newChunk->WriteComponentByHash(newIndex, GetTypeInfo<T>().hash, &value, sizeof(T));

	// 古いチャンクから削除（swap-pop）
	Entity movedEntity = oldChunk->RemoveEntity(entity);

	// m_entityToChunk マップを更新
	m_entityToChunk[entity] = newChunk;
	m_entityToArchetype[entity] = newArch;

	// もし oldChunk の末尾要素が idx に移動していたら、その移動した Entity の index 情報を更新する
	if (movedEntity != INVALID_ENTITY)
	{
		// movedEntity は oldChunk の oldIndex に移動したので
		// m_entityToChunk[movedEntity] は oldChunk を指しているはず -> index を oldIndex に更新する
		m_entityToChunk[movedEntity] = oldChunk;
	}
}
