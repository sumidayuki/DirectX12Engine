#include "GuardSystem.h"
#include "GuardState.h"
#include "PlayerTag.h"

void GuardSystem::Update(World& world)
{
	View<Transform, GuardState, Damageable, Animator, Input, HP> view(world);
	for (auto [entity, transform, guardState, damageable, animator, input, hp] : view)
	{
		if (!guardState.isInit)
		{
			bool isPlayer = world.HasComponent<PlayerTag>(entity);
			std::string name = isPlayer ? "Player" : "Enemy";
			guardState.bar = world.GetComponent<Slider>(UIManager::GetInstance()->GetUIObject(HashString("MainSceneUI"), HashString((name + "_guard_bar").c_str())));
			guardState.bar->minValue = 0;
			guardState.bar->maxValue = guardState.shieldMaxHealth;
			guardState.bar->value = guardState.shieldHealth;
		}

		guardState.bar->value = guardState.shieldHealth;

		guardState.isGuarding = input.isGuard;

		if (guardState.isBroken)
		{
			guardState.shieldHealth += 30 * Time::GetDeltaTime();
			guardState.shieldHealth = std::min(guardState.shieldHealth, guardState.shieldMaxHealth);
			guardState.isGuarding = true;
			hp.isSuperInvincible = false;

			if (guardState.shieldHealth >= guardState.shieldMaxHealth)
			{
				guardState.shieldHealth = guardState.shieldMaxHealth;
				guardState.isBroken = false;
			}

			continue;
		}

		hp.isSuperInvincible = guardState.isGuarding;

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
				guardState.shieldHealth -= damage.damage;

				animator.isLoop = false;
				AnimationSystem::Play(animator, "Guard_Impact", true);

				// シールドが破壊された場合の処理
				if (guardState.shieldHealth <= 0)
				{
					guardState.shieldHealth = 0;
					input.isGuard = false;
					guardState.isBroken = true;
					// シールド破壊アニメーションを再生
					animator.isLoop = false;
					AnimationSystem::Play(animator, "Hit_00");
				}

				guardState.shieldBreakTimer = 3.0f;

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

		// シールドが破壊されている場合、徐々に回復させる
		if (guardState.shieldBreakTimer <= 0 && guardState.shieldHealth < guardState.shieldMaxHealth)
		{
			guardState.shieldHealth += 2 * Time::GetDeltaTime();
			guardState.shieldHealth = std::min(guardState.shieldHealth, guardState.shieldMaxHealth);
		}

		guardState.shieldBreakTimer -= Time::GetDeltaTime();
	}
}
