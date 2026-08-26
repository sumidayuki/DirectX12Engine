#include "Precompiled.h"
#include "PlayerSystem.h"
#include "PlayerTag.h"
#include "PlayerCamera.h"
#include "ScopedProfiler.h"
#include "Arrow.h"
#include "BattleCamera.h"
#include "GuardState.h"
#include "CharacterImporter.h"
#include "LocomotionUtility.h"

bool PlayerSystem::ProcessTurn(World& world, Transform& transform, Animator& animator, LocomotionData& loco)
{
	if (loco.state == LocomotionState::Turning)
	{
		if (!animator.isPlaying)
		{
			transform.rotation = loco.turnTargetRot;
			transform.dirty = true;

			TransformAPI::EvaluateImmediate(world, transform);

			return false;
		}

		return true;
	}

	return false;
}

void PlayerSystem::Move(World& world, Transform& transform, Input& input, Animator& anim, LocomotionData& loco)
{
	if (ProcessTurn(world, transform, anim, loco))
	{
		return;
	}

	Vector3 camForward = m_cameraTransform->rotation * Vector3::forward;
	Vector3 camRight = m_cameraTransform->rotation * Vector3::right;

	camForward.y = 0;
	camRight.y = 0;

	camForward = camForward.Normalized();
	camRight = camRight.Normalized();

	Vector3 moveDirection = camForward * input.direction.y + camRight * input.direction.x;

	std::string targetClip = "Idle";
	const float moveThreshold = 0.01f;

	loco.currentVelocity = Vector3::Lerp(loco.currentVelocity, moveDirection, Time::GetDeltaTime() * 15.0f);

	if (loco.currentVelocity.SqrMagnitude() > 0.05f)
	{
		m_currentSpeed = input.dash ? RunSpeed : WalkSpeed;

		if (moveDirection.SqrMagnitude() > moveThreshold)
		{
			Vector3 currentDir = loco.currentVelocity.Normalized();
			Vector3 inputDir = moveDirection.Normalized();

			float angle = LocomotionUtility::CalculateMoveAngle(currentDir, inputDir);

			if (Mathf::Abs(angle) > 135.0f)
			{
				std::string turnClip = (angle > 0) ? "Turn_Right_180" : "Turn_Left_180";

				if (anim.clips.count(turnClip) > 0)
				{
					loco.state = LocomotionState::Turning;
					loco.turnTargetRot = Quaternion::LookRotation(inputDir, Vector3::up);
					loco.currentVelocity = inputDir;

					AnimationSystem::Play(anim, turnClip);
					anim.isLoop = false;

					return;
				}
			}
			else
			{
				targetClip = input.dash ? "Run_Forward" : "Walk_Forward";

				Quaternion targetRotation = Quaternion::LookRotation(inputDir, Vector3::up);

				transform.rotation = Quaternion::Slerp(transform.rotation, targetRotation, Time::GetDeltaTime() * 10.0f);

				TransformAPI::Translate(transform, inputDir * m_currentSpeed * Time::GetDeltaTime());

				loco.state = LocomotionState::Moving;
			}
		}
		else
		{
			targetClip = anim.currentClipName;
			loco.state = LocomotionState::Moving;
		}
	}
	else
	{
		loco.state = LocomotionState::Idle;
	}

	if (anim.currentClipName != targetClip)
	{
		AnimationSystem::Play(anim, targetClip);
		anim.isLoop = true;
	}
}

void PlayerSystem::DrawArrow(Transform& transform, float speed, float damage, Animator& anim, World& world)
{
	Vector3 forward = transform.rotation * Vector3::forward;
	forward.y = 0;
	forward = forward.Normalized();

	Vector3 pos = TransformAPI::GetPosition(*m_bowTransform);

	Entity arrow = world.CreateWithModel(L"Assets/Arrow.fbx", nullptr, pos, Quaternion::LookRotation(forward));

	Projectile projectile;
	projectile.lifeTime = 1.0f;
	projectile.speed = speed;
	projectile.velocity = forward * projectile.speed;

	world.AddComponent<Projectile>(arrow, projectile);

	Attackable attackable;
	attackable.damage = damage;
	attackable.damageType = (anim.currentClipName == "Attack_00") ? DamageType::Normal : DamageType::Heavy;

	world.AddComponent<Attackable>(arrow, attackable);

	Collider collider;
	collider.type = ColliderType::Sphere;
	collider.radius = 10.0f;
	collider.isTrigger = true;

	world.AddComponent<Collider>(arrow, collider);
	world.AddComponent<Arrow>(arrow, Arrow{});
}

void PlayerSystem::Start(World& world)
{
	Entity camera = world.FindEntityOfType<PlayerCamera>();

	m_cameraTransform = world.GetComponent<Transform>(camera);
	m_playerCamera = world.GetComponent<PlayerCamera>(camera);

	m_currentSpeed = WalkSpeed;
	m_bowTransform = nullptr;
	m_gameManager = nullptr;
}

void PlayerSystem::Update(World& world)
{
	View<PlayerTag, Transform, Input, Animator, MoveState, Attackable, HP, Damageable, GuardState, LocomotionData, RollingState> view(world);

	for (auto [entity, playerTag, transform, input, animator, state, attackable, hp, damageable, guard, loco, rolling] : view)
	{
		if (hp.isDeath)
		{
			if (animator.currentClipName != "Death")
			{
				animator.isLoop = false;
				AnimationSystem::Play(animator, "Death");
			}

			if (!animator.isPlaying)
			{
				SceneManager::ChangeScene("Title");
			}

			continue;
		}

		if (animator.currentClipName == "Hit_00" && animator.isPlaying)
		{
			continue;
		}

		if (!m_bowTransform)
		{
			Entity right = world.CreateEntity();

			BoneSocket socket;
			socket.targetEntity = entity;
			socket.targetBoneName = "mixamorig:Left_arch2";

			world.AddComponent<BoneSocket>(right, socket);

			m_bowTransform = world.GetComponent<Transform>(right);
		}

		const MoveData& currentMove = CharacterInfoRegistry::GetInstance()->GetMoveById(state.name, state.currentMoveId);

		switch (currentMove.type)
		{
		case MoveType::Idle:
		{
			rolling.isRolling = false;
			hp.isInvincible = false;

			Move(world, transform, input, animator, loco);

			break;
		}

		case MoveType::Rolling:
		{
			const RollingParams& params = std::get<RollingParams>(currentMove.params);

			if (!rolling.isRolling)
			{
				Vector3 rollDir = transform.rotation * Vector3::forward;
				rollDir.y = 0;
				rollDir = rollDir.Normalized();

				transform.rotation = Quaternion::LookRotation(rollDir, Vector3::up);

				rolling.direction = rollDir;
				rolling.isRolling = true;

				hp.isInvincible = false;
				loco.currentVelocity = Vector3(0, 0, 0);

				AnimationSystem::Play(animator, params.animationName, false);
				animator.isLoop = false;
			}

			float progress = (currentMove.duration > 0.0f) ? state.timer / currentMove.duration : 0.0f;

			hp.isInvincible = progress >= params.invincibleStart && progress <= params.invincibleEnd;

			float easedSpeed = params.moveSpeed * Mathf::Max(0.0f, 1.0f - progress * progress);

			TransformAPI::Translate(transform, rolling.direction * easedSpeed * Time::GetDeltaTime());

			break;
		}

		case MoveType::Guard:
		{
			rolling.isRolling = false;
			hp.isInvincible = false;
			loco.currentVelocity = Vector3(0, 0, 0);

			break;
		}

		case MoveType::Attack:
		{
			rolling.isRolling = false;
			hp.isInvincible = false;

			const AttackParams& params = std::get<AttackParams>(currentMove.params);

			loco.currentVelocity = Vector3(0, 0, 0);
			animator.isLoop = false;

			if (!m_gameManager)
			{
				m_gameManager = world.GetSystem<GameManagerSystem>();
			}

			Entity enemy = m_gameManager->GetEnemy();
			Transform* enemyTransform = world.GetComponent<Transform>(enemy);

			if (enemyTransform)
			{
				Vector3 toEnemy = enemyTransform->position - transform.position;
				toEnemy.y = 0;

				if (toEnemy.SqrMagnitude() > 0.001f)
				{
					Vector3 targetDir = toEnemy.Normalized();
					Quaternion targetRot = Quaternion::LookRotation(targetDir, Vector3::up);

					if (state.timer < 0.05f)
					{
						transform.rotation = targetRot;
					}
					else
					{
						transform.rotation = Quaternion::Slerp(transform.rotation, targetRot, Time::GetDeltaTime() * 20.0f);
					}
				}
			}

			if (!state.isAnimed)
			{
				AnimationSystem::Play(animator, params.animationName, true);
				state.isAnimed = true;
			}

			if (state.canHit && !state.hitConfirm)
			{
				bool canFire = true;

				if (enemyTransform)
				{
					Vector3 forward = transform.rotation * Vector3::forward;
					Vector3 targetDir = enemyTransform->position - transform.position;

					targetDir.y = 0;
					targetDir = targetDir.Normalized();

					transform.dirty = true;

					float dot = Vector3::Dot(forward.Normalized(), targetDir);

					if (dot < 0.95f)
					{
						canFire = false;
					}
				}

				if (canFire)
				{
					switch (currentMove.moveId)
					{
					case "attack-normal-1"_h:
					case "attack-normal-2"_h:
					case "attack-normal-3"_h:
						DrawArrow(transform, 2000.0f, params.damage, animator, world);
						state.hitConfirm = true;
						break;

					default:
						break;
					}
				}
			}

			break;
		}
		}
	}
}