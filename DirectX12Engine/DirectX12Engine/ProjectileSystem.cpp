#include "ProjectileSystem.h"

void ProjectileSystem::Update(ComponentManager& cm, World& world)
{
	View<Transform, Projectile> view(cm);

	for (auto [entity, transform, projectile] : view)
	{
		projectile.velocity = Vector3(transform.rotation * Vector3::forward) * projectile.speed;

		projectile.velocity.y -= 9.8f;

		TransformSystem::Translate(transform, projectile.velocity * Time::GetDeltaTime());

		TransformSystem::SetLocalRotation(transform, Quaternion::LookRotation(projectile.velocity.Normalized()));

		projectile.lifeTime -= Time::GetDeltaTime();
		if (projectile.lifeTime <= 0.0f)
		{
			world.DestroyEntity(entity);
		}
	}
}
