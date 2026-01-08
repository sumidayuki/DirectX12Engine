#pragma once
#include "Archetype.h"

/// <summary>
/// このクラスは同一ArchetypeのEntityデータをSoA（Structure of Arrays）形式で
/// 連続メモリに保持・管理するクラスです。
/// 各コンポーネント型ごとに連続した配列を持ち、キャッシュ効率を向上させます。
/// </summary>
class Chunk
{
private:
	const Archetype* m_archetype;						// このChunkが持つArchetypeレイアウトの参照
	std::vector<uint8_t*> m_componentArrays;			// SoA: 各コンポーネント型ごとの連続バッファ
	size_t m_capacity;									// 最大Entity数
	size_t m_count;										// 現在のEntity数
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
	void FreeArrays();

public:
	size_t FindEntityIndex(Entity e) const;

	size_t GetCount() const { return m_count; }

	size_t GetCapacity() const { return m_capacity; }

	bool HasSpace() const noexcept { return m_count < m_capacity; }

	Entity GetEntity(size_t index) const { return m_entities[index]; }
	
	size_t AddEntity(Entity entity);

	void* GetComponentPtrByHash(size_t index, uint64_t hash);

	void WriteComponentByHash(size_t index, uint64_t hash, const void* src);

	const void* ReadComponentByHash(size_t index, uint64_t hash) const;

	/// <summary>
	/// SoA用: 指定したコンポーネント型の配列先頭ポインタを取得します。
	/// これにより連続したコンポーネントデータへの効率的なアクセスが可能になります。
	/// </summary>
	template<ComponentType T>
	T* GetComponentArray()
	{
		auto it = m_typeIndexMap.find(GetTypeInfo<T>().hash);
		if (it == m_typeIndexMap.end())
		{
			return nullptr;
		}
		return reinterpret_cast<T*>(m_componentArrays[it->second]);
	}

	template<ComponentType T>
	const T* GetComponentArray() const
	{
		auto it = m_typeIndexMap.find(GetTypeInfo<T>().hash);
		if (it == m_typeIndexMap.end())
		{
			return nullptr;
		}
		return reinterpret_cast<const T*>(m_componentArrays[it->second]);
	}

	template<ComponentType T>
	T* AddComponent(Entity entity, const T& value)
	{
		auto it = m_typeIndexMap.find(GetTypeInfo<T>().hash);
		if (it == m_typeIndexMap.end())
		{
			assert(0);
		}

		size_t typeIdx = it->second;
		size_t index = FindEntityIndex(entity);
		if (index == SIZE_MAX)
		{
			assert(0);
		}

		// SoA: コンポーネント配列内の位置を計算
		const TypeInfo& typeInfo = m_archetype->types[typeIdx];
		uint8_t* dst = m_componentArrays[typeIdx] + typeInfo.size * index;
		GetTypeInfo<T>().copy_construct(dst, &value);

		return reinterpret_cast<T*>(dst);
	}

	template<ComponentType T>
	T* GetComponent(Entity entity)
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

		// SoA: コンポーネント配列内の位置を計算
		size_t typeIdx = it->second;
		const TypeInfo& typeInfo = m_archetype->types[typeIdx];
		return reinterpret_cast<T*>(m_componentArrays[typeIdx] + typeInfo.size * index);
	}

	template<ComponentType T>
	T* GetComponentByIndex(size_t index)
	{
		auto it = m_typeIndexMap.find(GetTypeInfo<T>().hash);
		if (it == m_typeIndexMap.end())
		{
			return nullptr;
		}

		if (index >= m_count)
		{
			return nullptr;
		}

		// SoA: インデックスで直接アクセス
		size_t typeIdx = it->second;
		const TypeInfo& typeInfo = m_archetype->types[typeIdx];
		return reinterpret_cast<T*>(m_componentArrays[typeIdx] + typeInfo.size * index);
	}

	Entity RemoveEntity(Entity entity);
};