#pragma once

#include "Entity.h"
#include <vector>
#include "ArchetypeManager.h"
#include "Chunk.h"

template<typename... Components>
class View
{
private:
    std::vector<Chunk*> m_chunks;

public:
    View(World& world)
        : m_chunks(world.GetArchetypeManager().GetChunks<Components...>())
    {
    }

    class Iterator
    {
    private:
        std::vector<Chunk*>::iterator m_chunkIt;
        std::vector<Chunk*>::iterator m_chunkEnd;
        size_t m_entityIndex;

    private:
        void SkipToNextValid()
        {
            while (m_chunkIt != m_chunkEnd && m_entityIndex >= (*m_chunkIt)->GetCount())
            {
                m_chunkIt++;
                m_entityIndex = 0;
            }
        }

    public:
        // begin—p
        Iterator(std::vector<Chunk*>::iterator begin, std::vector<Chunk*>::iterator end)
            : m_chunkIt(begin), m_chunkEnd(end), m_entityIndex(0)
        {
            SkipToNextValid();
        }

        // end—p
        Iterator(std::vector<Chunk*>::iterator end)
            : m_chunkIt(end), m_chunkEnd(end), m_entityIndex(0)
        {
        }

        std::tuple<Entity, Components&...> operator*()
        {
            Chunk* currentChunk = *m_chunkIt;
            Entity entity = currentChunk->GetEntity(m_entityIndex);

            return std::tuple<Entity, Components&...>(
                entity,
                *currentChunk->GetComponent<Components>(entity)...
            );
        }

        Iterator& operator++()
        {
            m_entityIndex++;
            SkipToNextValid();
            return *this;
        }

        bool operator!=(const Iterator& other) const
        {
            return m_chunkIt != other.m_chunkIt || m_entityIndex != other.m_entityIndex;
        }
    };

public:
    Iterator begin()
    {
        return Iterator(m_chunks.begin(), m_chunks.end());
    }

    Iterator end()
    {
        return Iterator(m_chunks.end());
    }
};