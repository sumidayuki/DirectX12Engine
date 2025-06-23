#pragma once

class ProtoType
{
	using AddFunc = std::function<void(Entity, ComponentManager&)>;

private:
	std::vector<AddFunc> m_addFuncs;

public:
	template <typename T>
	void Add(const T& comp)
	{
		m_addFuncs.push_back
		(
			[comp](Entity e, ComponentManager& cm) 
			{
				cm.AddComponent<T>(e, comp);
			}
		);
	}

	void Instantiate(Entity e, ComponentManager& cm) const;
};