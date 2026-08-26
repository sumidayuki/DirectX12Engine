#include "PhysicsIntegrator.h"

void PhysicsIntegrator::Step(World& world, float deltaTime)
{
	const Vector3 gravity(0.0f, -9.81f, 0.0f);

	View<Rigidbody, Transform> view(world);

	for (auto [entity, rb, transform] : view)
	{
		if (rb.isKinematic)
		{
			continue;
		}

		// 重力
		if (rb.useGravity)
		{
			rb.velocity += gravity * deltaTime;
		}

		// 線形抵抗
		if (rb.drag > 0.0f)
		{
			const float dragFactor =
				std::max(0.0f, 1.0f - rb.drag * deltaTime);

			rb.velocity *= dragFactor;
		}

		// 角抵抗
		if (rb.angularDrag > 0.0f)
		{
			const float angularDragFactor =
				std::max(0.0f, 1.0f - rb.angularDrag * deltaTime);

			rb.angularVelocity *= angularDragFactor;
		}

		// 位置を積分
		transform.position += rb.velocity * deltaTime;

		transform.dirty = true;
	}
}
