#include "ProjectileSystem.h"

void ProjectileSystem::Update(World& world)
{
	View<Transform, Projectile> view(world);

	for (auto [entity, transform, projectile] : view)
	{
		// 単純な重力のシミュレーション
		projectile.velocity.y -= 9.8f * Time::GetDeltaTime();

		// 速度に基づいて位置を更新
		Vector3 moveAmount = projectile.velocity * Time::GetDeltaTime();

		TransformAPI::Translate(transform, moveAmount);

		// 速度の方向に向ける
		if (projectile.velocity.SqrMagnitude() > 0.001f)
		{
			TransformAPI::SetLocalRotation(transform, Quaternion::LookRotation(projectile.velocity.Normalized()));
		}

		// ライフタイムの減少と寿命切れの処理
		projectile.lifeTime -= Time::GetDeltaTime();
		if (projectile.lifeTime <= 0.0f)
		{
			world.DestroyEntity(entity);
		}
	}
}
