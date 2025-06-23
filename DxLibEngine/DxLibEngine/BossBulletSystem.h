#pragma once
#include "TransformSystem.h"

class BossBulletSystem : public System
{
private:
	TransformSystem* m_transformSystem;

private:
	void Start(ComponentManager& cm, World& world) override;

	void Update(ComponentManager& cm, World& world) override;
};