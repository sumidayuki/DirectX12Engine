#include "GuardSystem.h"
#include "GuardState.h"

void GuardSystem::Update(World& world)
{
	View<Transform, GuardState, Damageable, Animator> view(world);
	for (auto [entity, transform, guardState, damageable, animator] : view)
	{
		if (guardState.isGuarding)
		{
			// ガードのスタートアニメーションを再生して、再生が終わっていたらループアニメーションに切り替える
			if (animator.currentClipName != "Guard_Start" && animator.currentClipName != "Guard_Idle" && animator.currentClipName != "Guard_Impact")
			{
				animator.isLoop = false;
				AnimationSystem::Play(animator, "Guard_Start", false);
			}
			else if ((animator.currentClipName == "Guard_Start" || animator.currentClipName == "Guard_Impact") && !animator.isPlaying)
			{
				animator.isLoop = true;
				AnimationSystem::Play(animator, "Guard_Idle", true);
			}

			// シールドが有効な場合、ダメージを軽減する
			while (!damageable.damageQueue.empty())
			{
				Damage damage = damageable.damageQueue.front();
				// シールドの耐久値を減少させる
				guardState.shieldHealth -= damage.damage * 0.5f; // ダメージの50%をシールドに吸収させる

				animator.isLoop = false;
				AnimationSystem::Play(animator, "Guard_Impact", true);

				// シールドが破壊された場合の処理
				if (guardState.shieldHealth <= 0)
				{
					guardState.shieldHealth = 0;
					// シールド破壊アニメーションを再生
					animator.isLoop = false;
					AnimationSystem::Play(animator, "Shield_Break", true);
				}

				// ダメージはシールドで防いだため、HPには影響しない
				damageable.damageQueue.pop();
			}

			Entity enemy = world.GetSystem<GameManagerSystem>()->GetEnemy();
			Transform* enemyTransform = world.GetComponent<Transform>(enemy);

			if (enemyTransform)
			{
				// 敵への方向を計算（高さYは無視）
				Vector3 toEnemy = enemyTransform->position - transform.position;
				toEnemy.y = 0;

				if (toEnemy.SqrMagnitude() > 0.001f)
				{
					Vector3 targetDir = toEnemy.Normalized();
					Quaternion targetRot = Quaternion::LookRotation(targetDir, Vector3::up);
					transform.rotation = Quaternion::Slerp(transform.rotation, targetRot, Time::GetDeltaTime() * 20.0f);
				}
			}
		}
		else
		{
			// シールドが破壊されている場合、徐々に回復させる
			if (guardState.shieldHealth < guardState.shieldMaxHealth)
			{
				guardState.shieldHealth += 10 * Time::GetDeltaTime();
				guardState.shieldHealth = std::min(guardState.shieldHealth, guardState.shieldMaxHealth);
			}
		}
	}
}
