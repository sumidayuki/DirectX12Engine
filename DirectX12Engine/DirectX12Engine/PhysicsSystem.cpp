#include "Precompiled.h"
#include "PhysicsSystem.h"
#include "World.h"
#include "Rigidbody.h"
#include "Transform.h"
#include "Time.h"

void PhysicsSystem::Update(World& world)
{
    float dt = Time::GetDeltaTime();
    Vector3 gravity(0, -9.81f, 0);

    View<Rigidbody, Transform> view(world);

    for (auto [entity, rb, transform] : view)
    {
        if (rb.isKinematic) continue;

        // Apply Gravity
        if (rb.useGravity)
        {
            rb.velocity += gravity * dt;
        }

        // Apply Drag (Linear)
        if (rb.drag > 0.0f)
        {
            rb.velocity *= (1.0f - rb.drag * dt);
        }

        // Apply Angular Drag
        if (rb.angularDrag > 0.0f)
        {
            rb.angularVelocity *= (1.0f - rb.angularDrag * dt);
        }

        // Integrate Position
        transform.position += rb.velocity * dt;
        transform.dirty = true;
    }
}
