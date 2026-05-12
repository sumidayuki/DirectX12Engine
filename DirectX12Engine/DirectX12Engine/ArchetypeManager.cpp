#include "ArchetypeManager.h"
#include "FNV-1a.h"

void ArchetypeManager::Clear()
{
	m_entityToChunk.clear();
	m_entityToArchetype.clear();
	m_chunks.clear();
	m_archetypes.clear();
}

const Archetype* ArchetypeManager::GetOrCreateArchetype(std::vector<TypeInfo> typeList)
{
	Archetype temp(typeList);

	// Šù‘¶‚ÌArchetype‚ğ’T‚µ‚Ü‚·B
	for (auto& existing : m_archetypes)
	{
		if (*existing == temp)
		{
			return existing.get();
		}
	}

	// V‹Kì¬
	m_archetypes.push_back(std::make_unique<Archetype>(std::move(temp)));
	return m_archetypes.back().get();
}

Chunk* ArchetypeManager::GetOrCreateChunk(const Archetype* archetype)
{
	uint64_t hash = HashArchetype(*archetype);

	auto& chunks = m_chunks[hash];
	for (auto& c : chunks)
	{
		if (c && c->HasSpace())
		{
			return c.get();
		}
	}

	chunks.push_back(std::make_unique<Chunk>(archetype));
	return chunks.back().get();
}

Entity ArchetypeManager::AddEntity(const Archetype* archetype, Entity entity)
{
	Chunk* chunk = GetOrCreateChunk(archetype);
	chunk->AddEntity(entity);
	m_entityToChunk[entity] = chunk;
	m_entityToArchetype[entity] = archetype;
	return entity;
}

void ArchetypeManager::DestroyEntity(Entity entity)
{
	auto it = m_entityToChunk.find(entity);
	if (it == m_entityToChunk.end()) return;

	it->second->RemoveEntity(entity);
	m_entityToChunk.erase(entity);
	m_entityToArchetype.erase(entity);
}

uint64_t ArchetypeManager::HashArchetype(const Archetype& archetype) const noexcept
{
	return FNV1a_Hash<uint64_t>(archetype.types.data(), archetype.types.size() * sizeof(TypeInfo));
}
