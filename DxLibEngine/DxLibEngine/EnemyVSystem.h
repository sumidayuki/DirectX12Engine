#pragma once
#include "TransformSystem.h"

class EnemyVSystem : public System
{
private:
	TransformSystem* m_transformSystem;

public:
	void Start(ComponentManager& cm, World& world) override;

	void Update(ComponentManager& cm, World& world) override;
};