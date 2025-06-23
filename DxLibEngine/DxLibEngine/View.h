#pragma once

template<typename First, typename... Rest>
class View
{
private:
	std::vector<Entity> m_entities;
	ComponentManager& m_cm;

public:
	View(ComponentManager& cm)
		: m_cm(cm)
	{
		Update();
	}

	void Update()
	{
		m_entities.clear();

		const auto& firstMap = m_cm.GetAllComponents<First>();

		for (const auto& [entity, _] : firstMap)
		{
			if (!entity.enabled) continue;
			if (m_cm.HasComponents<First, Rest...>(entity))
			{
				m_entities.push_back(entity);
			}
		}
	}

	// æ“¾‚Ì‚İ‚µ‚½‚¢ê‡—p
	const std::vector<Entity>& GetEntities() const { return m_entities; }

	struct Iterator
	{
		size_t index;
		View* view;

		auto operator*() const
		{
			Entity e = view->m_entities[index];
			return std::tuple<Entity, First&, Rest&...>(
				e,
				*(view->m_cm.GetComponent<First>(e)),
				*(view->m_cm.GetComponent<Rest>(e))...
			);
		}

		Iterator& operator++() { ++index; return *this; }
		bool operator!=(const Iterator& other) const { return index != other.index; }
	};

	Iterator begin() { return Iterator{ 0, this }; }
	Iterator end() { return Iterator{ m_entities.size(), this }; }
};
