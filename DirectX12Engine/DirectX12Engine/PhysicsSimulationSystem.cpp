#include "PhysicsSimulationSystem.h"

void PhysicsSimulationSystem::Update(World& world)
{
	const float dt = Time::GetDeltaTime();
	const float subDt = dt / SUBSTEP_COUNT;

	m_collisionSolver.BeginFrame(world);

	for (int i = 0; i < SUBSTEP_COUNT; ++i)
	{
		m_integrator.Step(world, subDt);
		
		m_collisionSolver.Step(world);
	}

	m_collisionSolver.EndFrame(world);
}
