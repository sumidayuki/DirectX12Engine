#pragma once

#include "Entity.h"
#include "ComponentManager.h"
#include <vector>
#include <tuple>
#include <type_traits>

template<typename... Components>
class View
{
private:
    ComponentManager& m_cm;
    const std::deque<Entity>& m_entityList;

public:
    View(ComponentManager& cm)
        : m_cm(cm),
        m_entityList(cm.GetEntities<std::tuple_element_t<0, std::tuple<Components...>>>()) {
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
        size_t firstValidIndex = 0;
        while (firstValidIndex < m_entityList.size() &&
            !m_cm.HasComponents<Components...>(m_entityList[firstValidIndex]))
        {
            ++firstValidIndex;
        }
        return Iterator(firstValidIndex, this);
    }

    Iterator end() const { return Iterator(m_entityList.size(), this); }
};