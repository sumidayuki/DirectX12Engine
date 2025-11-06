#pragma once

#include "Entity.h"
#include "ComponentManager.h"
#include <vector>

template<typename... Components>
class View
{
private:
    ComponentManager& m_cm;
    std::vector<Entity> m_entityList;

public:
    View(ComponentManager& cm)
        : m_cm(cm)
    {
        const std::vector<Entity>& sourceList = cm.GetEntities<std::tuple_element_t<0, std::tuple<Components...>>>();
        for (Entity e : sourceList)
        {
            if (m_cm.HasComponents<Components...>(e))
            {
                m_entityList.push_back(e);
            }
        }
    }

    class Iterator
    {
    private:
        size_t m_index;
        const View* m_view;

    public:
        Iterator(size_t index, const View* view) : m_index(index), m_view(view) {}

        auto operator*() const
        {
            Entity currentEntity = m_view->m_entityList[m_index];
            return std::tuple<Entity, Components&...>(
                currentEntity,
                *m_view->m_cm.GetComponent<Components>(currentEntity)...
            );
        }

        Iterator& operator++()
        {
            do {
                ++m_index;
            } while (m_index < m_view->m_entityList.size() &&
                !m_view->m_cm.HasComponents<Components...>(m_view->m_entityList[m_index]));
            return *this;
        }

        bool operator!=(const Iterator& other) const { return m_index != other.m_index; }
    };

    Iterator begin() const
    {
        if (m_entityList.empty())
        {
            return Iterator(m_entityList.size(), this);
        }
        return Iterator(0, this);
    }

    Iterator end() const { return Iterator(m_entityList.size(), this); }
};