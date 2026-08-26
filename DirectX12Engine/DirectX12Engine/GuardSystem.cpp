#include "GuardSystem.h"
#include "GuardState.h"
#include "PlayerTag.h"
#include "CharacterInfoRegistry.h"
#include "MoveInput.h"

void GuardSystem::Update(World& world)
{
	View<Transform, GuardState, Damageable, Animator, Input, HP, MoveState, MoveInput> view(world);

	for (auto [entity, transform, guardState, damageable, animator, input, hp, moveState, moveInput] : view)
	{
		if (!guardState.isInit)
		{
			bool isPlayer = world.HasComponent<PlayerTag>(entity);
			std::string name = isPlayer ? "Player" : "Enemy";

			guardState.bar = world.GetComponent<Slider>(
				UIManager::GetInstance()->GetUIObject(
					HashString("MainSceneUI"),
					HashString((name + "_guard_bar").c_str())
				)
			);

			guardState.bar->minValue = 0;
			guardState.bar->maxValue = guardState.shieldMaxHealth;
			guardState.bar->value = guardState.shieldHealth;

			Effect* effect = AssetManager::GetInstance()->GetAsset<Effect>(
				AssetType::Effect,
				L"Assets/Effects/effect_archer-guard_guarding-00.efkefc"
			);

			Entity effectEntity = world.CreateEntity("GuardEffect");

			guardState.effectEntity = effectEntity;

			BoneSocket socket;
			socket.targetEntity = entity;
			socket.targetBoneName = "mixamorig:Hips";
			socket.offsetSpace = BoneSocketSpace::Target;
			socket.posOffset = Vector3(0, -30, 40);
			socket.scaleOffset = Vector3(75, 75, 75);
			socket.followScale = true;

			world.AddComponent<BoneSocket>(effectEntity, socket);

			EffectSource source;
			source.effect = effect;
			source.playOnAwake = false;
			source.followTransform = true;
			source.loop = false;
			source.lifeTime = INFINITY;
			source.color = Color::white;

			world.AddComponent<EffectSource>(effectEntity, source);

			guardState.isInit = true;
		}

		guardState.bar->value = guardState.shieldHealth;

		const MoveData& currentMove = CharacterInfoRegistry::GetInstance()->GetMoveById(
			moveState.name,
			moveState.currentMoveId
		);

		guardState.isGuarding = currentMove.type == MoveType::Guard && !guardState.isBroken;

		if (guardState.isBroken)
		{
			guardState.isGuarding = false;
			hp.isSuperInvincible = false;

			guardState.shieldHealth += 30.0f * Time::GetDeltaTime();

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
			EffectSource* effectSource = world.GetComponent<EffectSource>(guardState.effectEntity);

			float guardRate = guardState.shieldHealth / guardState.shieldMaxHealth;

			effectSource->color = GetGuardColor(guardRate);

			if (
				animator.currentClipName != "Guard_Start" &&
				animator.currentClipName != "Guard_Idle" &&
				animator.currentClipName != "Guard_Impact"
				)
			{
				animator.isLoop = false;

				AnimationSystem::Play(animator, "Guard_Start", false);
				EffectAPI::Play(effectSource);
			}
			else if (
				(animator.currentClipName == "Guard_Start" ||
					animator.currentClipName == "Guard_Impact") &&
				!animator.isPlaying
				)
			{
				animator.isLoop = true;

				AnimationSystem::Play(animator, "Guard_Idle", true);
			}

			while (!damageable.damageQueue.empty())
			{
				Damage damage = damageable.damageQueue.front();

				guardState.shieldHealth -= damage.damage;

				animator.isLoop = false;

				AnimationSystem::Play(animator, "Guard_Impact", true);

				effectSource->color = Color::white;

				if (guardState.shieldHealth <= 0.0f)
				{
					guardState.shieldHealth = 0.0f;
					guardState.isGuarding = false;
					guardState.isBroken = true;

					hp.isSuperInvincible = false;

					input.isGuard = false;

					moveInput.releasedKey = InputKey::Guard;

					animator.isLoop = false;

					AnimationSystem::Play(animator, "Hit_00");

					EffectAPI::Stop(effectSource);
				}

				guardState.shieldBreakTimer = 3.0f;

				damageable.damageQueue.pop();
			}

			Entity enemy = world.GetSystem<GameManagerSystem>()->GetEnemy();
			Transform* enemyTransform = world.GetComponent<Transform>(enemy);

			if (enemyTransform)
			{
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
			EffectSource* effectSource = world.GetComponent<EffectSource>(guardState.effectEntity);

			EffectAPI::Stop(effectSource);
		}

		if (guardState.shieldBreakTimer <= 0.0f && guardState.shieldHealth < guardState.shieldMaxHealth)
		{
			guardState.shieldHealth += 2.0f * Time::GetDeltaTime();
			guardState.shieldHealth = std::min(guardState.shieldHealth, guardState.shieldMaxHealth);
		}

		guardState.shieldBreakTimer -= Time::GetDeltaTime();
	}
}

Color GuardSystem::GetGuardColor(float rate) const
{
	rate = Mathf::Clamp(rate, 0.0f, 1.0f);

	const Color red = Color(1, 0, 0, 0.5f);
	const Color yellow = Color(1, 1, 0, 0.5f);
	const Color blue = Color(0, 0.5f, 1, 0.5f);

	if (rate >= 0.5f)
	{
		float t = (rate - 0.5f) * 2.0f;

		return Vector4::Lerp(yellow, blue, t);
	}

	float t = rate * 2.0f;

	return Vector4::Lerp(red, yellow, t);
}