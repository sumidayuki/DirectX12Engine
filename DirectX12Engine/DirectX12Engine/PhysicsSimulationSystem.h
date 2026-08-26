#pragma once

#include "PhysicsIntegrator.h"
#include "CollisionSolver.h"

class PhysicsSimulationSystem : public System
{
private:
    PhysicsIntegrator m_integrator;
    CollisionSolver m_collisionSolver;

	static constexpr int SUBSTEP_COUNT = 2;

private:
	void Update(World& world) override;
};