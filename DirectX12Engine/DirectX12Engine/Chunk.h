#pragma once
#include "Archetype.h"

/// <summary>
/// このクラスは同一ArchetypeのEntityデータを連続メモリで保持・管理するクラスです。
/// Archetypeのレイアウトを参照して各コンポーネントへアクセスします。
/// </summary>
class Chunk
{
private:
	const Archetype* m_archetype;							// このChunkが持つArchetypeレイアウトの参照
	uint8_t* m_buffer;										// 生バッファ
	size_t m_capacity;										// 最大Entity数
	size_t m_count;											// 現在のEntity数
	size_t m_bufferAlignment;
	std::vector<Entity> m_entities;											
	std::unordered_map<uint64_t, size_t> m_typeIndexMap;	

public:
	Chunk(const Archetype* archetype, size_t capacity = 64);
	~Chunk();

	Chunk(const Chunk&) = delete;
	Chunk& operator =(const Chunk&) = delete;
	Chunk(Chunk&& other) noexcept;
	Chunk& operator =(Chunk&& other) noexcept;

private:
	void MoveFrom(Chunk&& other);

public:
	size_t FindEntityIndex(Entity e) const;

	bool HasSpace() const noexcept { return m_count < m_capacity; }

	Entity GetEntity(size_t index) const { return m_entities[index]; }
	
	size_t AddEntity(Entity entity);

	void* GetComponentPtrByHash(size_t index, uint64_t hash);

	void WriteComponentByHash(size_t index, uint64_t hash, const void* src);

	const void* ReadComponentByHash(size_t index, uint64_t hash) const;

	template<ComponentType T>
	T* AddComponent(Entity entity, const T& value);

	template<ComponentType T>
	T* GetComponent(Entity entity);

	Entity RemoveEntity(Entity entity);
};

template<ComponentType T>
inline T* Chunk::AddComponent(Entity entity, const T& value)
{
	auto it = m_typeIndexMap.find(GetTypeInfo<T>().hash);
	if (it == m_typeIndexMap.end())
	{
		assert(0);
	}

	size_t typeIdx = it->second;
	size_t offset = m_archetype->offsets[typeIdx];
	size_t index = FindEntityIndex(entity);
	if (index == SIZE_MAX)
	{
		assert(0);
	}

	uint8_t* dst = m_buffer + offset + m_archetype->totalSize * index;
	std::memcpy(dst, &value, sizeof(T));

	return reinterpret_cast<T*>(dst);
}

template<ComponentType T>
inline T* Chunk::GetComponent(Entity entity)
{
	auto it = m_typeIndexMap.find(GetTypeInfo<T>().hash);
	if (it == m_typeIndexMap.end())
	{
		return nullptr;
	}

	size_t index = FindEntityIndex(entity);
	if (index == SIZE_MAX)
	{
		return nullptr;
	}

	size_t offset = m_archetype->offsets[it->second];
	return reinterpret_cast<T*>(m_buffer + offset + m_archetype->totalSize * index);
}
