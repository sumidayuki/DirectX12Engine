#include "Chunk.h"

Chunk::Chunk(const Archetype* archetype, size_t capacity)
	: m_archetype(archetype)
	, m_capacity(capacity)
	, m_count(0)
{
	// アーキタイプが設定されていない場合はエラーが出ます。
	if (!m_archetype)
	{
		throw std::invalid_argument("Chunk: archetype is null");
	}

	// capacityの数値0は無効の値なのでデフォルト値に設定します。
	if (m_capacity == 0)
	{
		m_capacity = 64;
	}

	// SoA: 各コンポーネント型ごとに別々のメモリを確保
	m_componentArrays.resize(m_archetype->types.size());
	
	for (size_t i = 0; i < m_archetype->types.size(); i++)
	{
		const TypeInfo& type = m_archetype->types[i];
		size_t arraySize = type.size * m_capacity;
		size_t alignment = std::max(type.alignment, static_cast<size_t>(alignof(void*)));
		
		// アライメント付きメモリの確保
#if defined(__cpp_aligned_new) || (_MSC_VER && _HAS_CXX17)
		m_componentArrays[i] = reinterpret_cast<uint8_t*>(
			::operator new(arraySize, std::align_val_t(alignment)));
#else
#if defined(_MSC_VER)
		m_componentArrays[i] = reinterpret_cast<uint8_t*>(_aligned_malloc(arraySize, alignment));
#else
		m_componentArrays[i] = reinterpret_cast<uint8_t*>(aligned_alloc(alignment, arraySize));
#endif
#endif
		if (!m_componentArrays[i])
		{
			// 既に確保したメモリを解放
			FreeArrays();
			throw std::bad_alloc();
		}
		
		// 型ハッシュからインデックスへのマッピング
		m_typeIndexMap[type.hash] = i;
	}

	// エンティティ配列をcapacity分確保しておきます。
	m_entities.reserve(m_capacity);
}

void Chunk::FreeArrays()
{
	if (!m_archetype) return;
	
	for (size_t i = 0; i < m_componentArrays.size(); i++)
	{
		if (m_componentArrays[i])
		{
			const TypeInfo& type = m_archetype->types[i];
			size_t alignment = std::max(type.alignment, static_cast<size_t>(alignof(void*)));
			
#if defined(__cpp_aligned_new) || (_MSC_VER && _HAS_CXX17)
			::operator delete(m_componentArrays[i], std::align_val_t(alignment));
#else
#if defined(_MSC_VER)
			_aligned_free(m_componentArrays[i]);
#else
			free(m_componentArrays[i]);
#endif
#endif
			m_componentArrays[i] = nullptr;
		}
	}
	m_componentArrays.clear();
}

Chunk::~Chunk()
{
	// 全アクティブコンポーネントのデストラクタを呼び出し
	for (size_t entityIdx = 0; entityIdx < m_count; ++entityIdx)
	{
		for (size_t typeIdx = 0; typeIdx < m_archetype->types.size(); ++typeIdx)
		{
			const TypeInfo& type = m_archetype->types[typeIdx];
			// SoA: 各コンポーネント配列内の位置を計算
			void* componentPtr = m_componentArrays[typeIdx] + type.size * entityIdx;
			type.destroy(componentPtr);
		}
	}

	FreeArrays();
}

Chunk::Chunk(Chunk&& other) noexcept
	: m_archetype(nullptr)
	, m_capacity(0)
	, m_count(0)
{
	MoveFrom(std::move(other));
}

Chunk& Chunk::operator=(Chunk&& other) noexcept
{
	if (this != &other)
	{
		// 既存コンポーネントのデストラクタ呼び出し
		for (size_t entityIdx = 0; entityIdx < m_count; ++entityIdx)
		{
			for (size_t typeIdx = 0; typeIdx < m_archetype->types.size(); ++typeIdx)
			{
				const TypeInfo& type = m_archetype->types[typeIdx];
				void* componentPtr = m_componentArrays[typeIdx] + type.size * entityIdx;
				type.destroy(componentPtr);
			}
		}
		
		FreeArrays();
		MoveFrom(std::move(other));
	}

	return *this;
}

void Chunk::MoveFrom(Chunk&& other)
{
	m_archetype = other.m_archetype;
	m_componentArrays = std::move(other.m_componentArrays);
	m_capacity = other.m_capacity;
	m_count = other.m_count;
	m_entities = std::move(other.m_entities);
	m_typeIndexMap = std::move(other.m_typeIndexMap);

	other.m_archetype = nullptr;
	other.m_capacity = 0;
	other.m_count = 0;
}

size_t Chunk::FindEntityIndex(Entity e) const
{
	for (size_t i = 0; i < m_entities.size(); ++i)
	{
		if (m_entities[i] == e)
			return i;
	}
	return SIZE_MAX;
}

size_t Chunk::AddEntity(Entity entity)
{
	if (!HasSpace())
	{
		assert(0);
	}

	m_entities.push_back(entity);
	return m_count++;
}

void* Chunk::GetComponentPtrByHash(size_t index, uint64_t hash)
{
	auto it = m_typeIndexMap.find(hash);
	if (it == m_typeIndexMap.end())
	{
		return nullptr;
	}

	size_t typeIndex = it->second;
	const TypeInfo& type = m_archetype->types[typeIndex];
	// SoA: コンポーネント配列内の位置を計算
	return m_componentArrays[typeIndex] + type.size * index;
}

void Chunk::WriteComponentByHash(size_t index, uint64_t hash, const void* src)
{
	void* dst = GetComponentPtrByHash(index, hash);
	if (!dst)
	{
		return;
	}
	const auto& t = m_archetype->types[m_typeIndexMap[hash]];
	t.copy_construct(dst, src);
}

const void* Chunk::ReadComponentByHash(size_t index, uint64_t hash) const
{
	auto it = m_typeIndexMap.find(hash);
	if (it == m_typeIndexMap.end())
	{
		return nullptr;
	}
	size_t typeIndex = it->second;
	const TypeInfo& type = m_archetype->types[typeIndex];
	// SoA: コンポーネント配列内の位置を計算
	return m_componentArrays[typeIndex] + type.size * index;
}

Entity Chunk::RemoveEntity(Entity entity)
{
	size_t idx = FindEntityIndex(entity);
	if (idx == SIZE_MAX) return INVALID_ENTITY;

	size_t last = m_count - 1;
	Entity moved = INVALID_ENTITY;

	// 削除するエンティティのコンポーネントのデストラクタを呼び出し
	for (size_t typeIdx = 0; typeIdx < m_archetype->types.size(); ++typeIdx)
	{
		const TypeInfo& type = m_archetype->types[typeIdx];
		// SoA: 各コンポーネント配列内の位置を計算
		void* componentPtr = m_componentArrays[typeIdx] + type.size * idx;
		type.destroy(componentPtr);
	}

	if (idx != last)
	{
		// 最後のエンティティのコンポーネントを削除位置に移動
		for (size_t typeIdx = 0; typeIdx < m_archetype->types.size(); ++typeIdx)
		{
			const TypeInfo& type = m_archetype->types[typeIdx];
			// SoA: 各コンポーネント配列内の位置を計算
			void* dst = m_componentArrays[typeIdx] + type.size * idx;
			void* src = m_componentArrays[typeIdx] + type.size * last;

			// ムーブコンストラクト
			type.move_construct(dst, src);
			// ソースのデストラクタ呼び出し
			type.destroy(src);
		}

		moved = m_entities[last];
		m_entities[idx] = moved;
	}
	
	m_entities.pop_back();
	--m_count;

	return moved;
}