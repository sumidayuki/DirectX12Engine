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
        Iterator(std::vector<Chunk*>::iterator begin, std::vector<Chunk*>::iterator end)
            : m_chunkIt(begin), m_chunkEnd(end), m_entityIndex(0)
        {
            SkipToNextValid();
        }

        Iterator(std::vector<Chunk*>::iterator end)
            : m_chunkIt(end), m_chunkEnd(end), m_entityIndex(0)
        {
        }

        bool operator!=(const Iterator& other) const
        {
            return m_chunkIt != other.m_chunkIt || m_entityIndex != other.m_entityIndex;
        }

        Iterator& operator++()
        {
            m_entityIndex++;
            SkipToNextValid();
            return *this;
        }

        std::tuple<Entity, Components&...> operator*()
        {
            Chunk* chunk = *m_chunkIt;
            return std::tuple<Entity, Components&...>(
                chunk->GetEntity(m_entityIndex),
                *chunk->template GetComponentByIndex<Components>(m_entityIndex)...
            );
        }
    };

    Iterator begin() { return Iterator(m_chunks.begin(), m_chunks.end()); }
    Iterator end()   { return Iterator(m_chunks.end()); }

    template<typename Func>
    void Each(Func&& func)
    {
        for (Chunk* chunk : m_chunks)
        {
            const size_t count = chunk->GetCount();
            if (count == 0) continue;

            auto arrays = std::make_tuple(chunk->template GetComponentArray<Components>()...);
            
            for (size_t i = 0; i < count; ++i)
            {
                Entity entity = chunk->GetEntity(i);
                func(entity, std::get<decltype(chunk->template GetComponentArray<Components>())>(arrays)[i]...);
            }
        }
    }

    template<typename Func>
    void EachComponent(Func&& func)
    {
        for (Chunk* chunk : m_chunks)
        {
            const size_t count = chunk->GetCount();
            if (count == 0) continue;

            auto arrays = std::make_tuple(chunk->template GetComponentArray<Components>()...);
            
            for (size_t i = 0; i < count; ++i)
            {
                func(std::get<decltype(chunk->template GetComponentArray<Components>())>(arrays)[i]...);
            }
        }
    }

    size_t Count() const
    {
        size_t total = 0;
        for (const Chunk* chunk : m_chunks)
        {
            total += chunk->GetCount();
        }
        return total;
    }

    bool Empty() const
    {
        for (const Chunk* chunk : m_chunks)
        {
            if (chunk->GetCount() > 0) return false;
        }
        return true;
    }
};
