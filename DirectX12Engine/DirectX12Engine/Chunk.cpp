#include "Chunk.h"

Chunk::Chunk(const Archetype* archetype, size_t capacity)
	: m_archetype(archetype)
	, m_capacity(capacity)
{
	// アーキタイプが設定されていない場合はエラーが出ます。
	if (!m_archetype)
	{
		throw std::invalid_argument("Chunk: archetype is null");
	}

	// capacityの数値0は向こうの値なのでデフォルト値に設定します。
	if (m_capacity == 0)
	{
		m_capacity = 64;
	}

	// アライメントとサイズ計算をします。
	size_t maxAlign = 1;
	for (const auto& type : m_archetype->types)
	{
		maxAlign = std::max(maxAlign, type.alignment);
	}
	m_bufferAlignment = std::max(maxAlign, static_cast<size_t>(alignof(void*)));

	const size_t totalBytes = m_archetype->totalSize * m_capacity;

	// アライメント付きメモリの確保を行います。
	// C++の aligned new が使える場合はそれを使用します。
#if defined(__cpp_aligned_new) || (_MSC_VER && _HAS_CXX17)
	m_buffer = reinterpret_cast<uint8_t*>(::operator new (totalBytes, std::align_val_t(m_bufferAlignment)));
#endif
	if (!m_buffer)
	{
		throw std::bad_alloc();
	}

	// エンティティ配列をcapacity分確保しておきます。
	m_entities.reserve(m_capacity);

	// Archetypeの型配列を走査して、type.hashをキーとして型インデックスを保存します。
	for (size_t i = 0; i < m_archetype->types.size(); i++)
	{
		m_typeIndexMap[m_archetype->types[i].hash] = i;
	}
}

Chunk::~Chunk()
{
	if (m_buffer)
	{
		// Call destructors for all active components
		for (size_t i = 0; i < m_count; ++i)
		{
			for (const auto& type : m_archetype->types)
			{
				void* componentPtr = m_buffer + m_archetype->offsets[m_typeIndexMap[type.hash]] + i * m_archetype->totalSize;
				type.destroy(componentPtr);
			}
		}

#if defined(__cpp_aligned_new) || (_MSC_VER && _HAS_CXX17)
		::operator delete(m_buffer, std::align_val_t(m_bufferAlignment));
#else
#if defined(_MSC_VER)
		_aligned_free(m_buffer);
#else
		free(m_buffer);
#endif
#endif
	}
}

Chunk::Chunk(Chunk&& other) noexcept
{
	MoveFrom(std::move(other));
}

Chunk& Chunk::operator=(Chunk&& other) noexcept
{
	if (this != &other)
	{
		// Destroy existing components before moving
		if (m_buffer)
		{
			for (size_t i = 0; i < m_count; ++i)
			{
				for (const auto& type : m_archetype->types)
				{
					void* componentPtr = m_buffer + m_archetype->offsets[m_typeIndexMap[type.hash]] + i * m_archetype->totalSize;
					type.destroy(componentPtr);
				}
			}
#if defined(__cpp_aligned_new) || (_MSC_VER && _HAS_CXX17)
			::operator delete(m_buffer, std::align_val_t(m_bufferAlignment));
#else
#if defined(_MSC_VER)
			_aligned_free(m_buffer);
#else
			free(m_buffer);
#endif
#endif
		}
		MoveFrom(std::move(other));
	}

	return *this;
}

void Chunk::MoveFrom(Chunk&& other)
{
	m_archetype = other.m_archetype;
	m_buffer = other.m_buffer;
	m_capacity = other.m_capacity;
	m_count = other.m_count;
	m_entities = std::move(other.m_entities);
	m_typeIndexMap = std::move(other.m_typeIndexMap);
	m_bufferAlignment = other.m_bufferAlignment;

	other.m_archetype = nullptr;
	other.m_buffer = nullptr;
	other.m_capacity = 0;
	other.m_count = 0;
	other.m_bufferAlignment = 0;
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
	size_t offset = m_archetype->offsets[typeIndex];
	return m_buffer + (index * m_archetype->totalSize) + offset;
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
	size_t offset = m_archetype->offsets[typeIndex];
	return m_buffer + (index * m_archetype->totalSize) + offset;
}

Entity Chunk::RemoveEntity(Entity entity)
{
	size_t idx = FindEntityIndex(entity);
	if (idx == SIZE_MAX) return INVALID_ENTITY;

	// Call destructors for the component being removed
	for (const auto& type : m_archetype->types)
	{
		void* componentPtr = m_buffer + m_archetype->offsets[m_typeIndexMap[type.hash]] + idx * m_archetype->totalSize;
		type.destroy(componentPtr);
	}

	size_t last = m_count - 1;
	Entity moved = INVALID_ENTITY;

	if (idx != last)
	{
		// Move the last entity's components to the removed entity's position
		for (size_t i = 0; i < m_archetype->types.size(); ++i)
		{
			const auto& type = m_archetype->types[i];
			size_t offset = m_archetype->offsets[i];

			void* dst = m_buffer + offset + idx * m_archetype->totalSize;
			void* src = m_buffer + offset + last * m_archetype->totalSize;

			// Destroy the component at destination before moving
			type.destroy(dst);
			// Move construct the component
			type.move_construct(dst, src);
		}

		moved = m_entities[last];
		m_entities[idx] = moved;
	}
	m_entities.pop_back();
	--m_count;

	return moved;
}