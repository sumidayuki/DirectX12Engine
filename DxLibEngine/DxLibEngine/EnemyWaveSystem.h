#pragma once
#include "TransformSystem.h"

class EnemyWaveSystem : public System
{
private:
	TransformSystem* m_transformSystem;

public:
	void Start(ComponentManager& cm, World& world) override;

	void Update(ComponentManager& cm, World& world) override;
};