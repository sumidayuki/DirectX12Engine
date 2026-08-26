#include "Precompiled.h"
#include "PlayerTag.h"
#include "CharacterImporter.h"
#include "LocomotionUtility.h"

bool EnemySystem::ProcessTurn(World& world, Transform& transform, Animator& animator, LocomotionData& loco)
{
	if (loco.state == LocomotionState::Turning)
	{
		if (!animator.isPlaying)
		{
			transform.rotation = loco.turnTargetRot;
			transform.dirty = true;
			loco.state = LocomotionState::Idle;

			TransformAPI::EvaluateImmediate(world, transform);

			return false;
		}

		return true;
	}

	return false;
}

void EnemySystem::Move(World& world, Enemy& enemy, AIAgent& aiAgent, Transform& transform, Animator& animator, LocomotionData& loco)
{
	aiAgent.updatePosition = true;
	aiAgent.updateRotation = false;

	Transform* targetTrans = world.GetComponent<Transform>(enemy.target);

	Vector3 targetPos = targetTrans->position;
	Vector3 dir = targetPos - transform.position;
	dir.y = 0;

	float currentSpeed = Vector3(aiAgent.velocity.x, 0, aiAgent.velocity.z).Magnitude();

	if (aiAgent.speed >= 200.0f && currentSpeed > 0.1f)
	{
		if (dir.SqrMagnitude() > 0.001f)
		{
			dir = dir.Normalized();

			Quaternion targetRot = Quaternion::LookRotation(dir);

			transform.rotation = Quaternion::Slerp(transform.rotation, targetRot, Time::GetDeltaTime() * 10.0f);
			transform.dirty = true;
		}

		if (animator.currentClipName != "Run")
		{
			AnimationSystem::Play(animator, "Run");
			animator.isLoop = true;
		}

		loco.state = LocomotionState::Moving;
	}
	else if (currentSpeed > 0.1f)
	{
		if (dir.SqrMagnitude() > 0.001f)
		{
			dir = dir.Normalized();

			Quaternion targetRot = Quaternion::LookRotation(dir);

			transform.rotation = Quaternion::Slerp(transform.rotation, targetRot, Time::GetDeltaTime() * 10.0f);
			transform.dirty = true;
		}

		Vector3 charForward = transform.rotation * Vector3::forward;
		charForward.y = 0;

		if (charForward.SqrMagnitude() > 0.001f)
		{
			charForward = charForward.Normalized();
		}

		Vector3 moveDir = Vector3(aiAgent.velocity.x, 0, aiAgent.velocity.z).Normalized();

		float angle = LocomotionUtility::CalculateMoveAngle(charForward, moveDir);

		std::string targetClip = LocomotionUtility::SelectDirectionalClip(
			angle,
			"Walk",
			"Walk_Backward",
			"Walk_Right",
			"Walk_Left"
		);

		if (animator.currentClipName != targetClip)
		{
			AnimationSystem::Play(animator, targetClip);
			animator.isLoop = true;
		}

		loco.state = LocomotionState::Moving;
	}
	else
	{
		if (ProcessTurn(world, transform, animator, loco))
		{
			return;
		}

		if (dir.SqrMagnitude() > 0.001f)
		{
			dir = dir.Normalized();

			Vector3 charForward = transform.rotation * Vector3::forward;
			charForward.y = 0;

			if (charForward.SqrMagnitude() > 0.001f)
			{
				charForward = charForward.Normalized();
			}

			float angleToTarget = LocomotionUtility::CalculateMoveAngle(charForward, dir);

			if (Mathf::Abs(angleToTarget) > 80.0f)
			{
				std::string turnClip = (angleToTarget > 0) ? "Turn_Left_90" : "Turn_Right_90";

				if (animator.clips.count(turnClip) > 0)
				{
					if (loco.state != LocomotionState::Turning)
					{
						Quaternion rot90 = Quaternion::AngleAxis(
							(angleToTarget > 0) ? 90.0f : -90.0f,
							Vector3::up
						);

						loco.turnTargetRot = transform.rotation * rot90;
					}

					loco.state = LocomotionState::Turning;

					AnimationSystem::Play(animator, turnClip);
					animator.isLoop = false;

					return;
				}
			}
		}

		if (animator.currentClipName != "Idle")
		{
			AnimationSystem::Play(animator, "Idle");
			animator.isLoop = true;
		}

		loco.state = LocomotionState::Idle;
	}
}

void EnemySystem::Miai(World& world, Enemy& enemy, AIAgent& aiAgent, Transform& transform, CharacterStatus& status)
{
	Transform* targetTrans = world.GetComponent<Transform>(enemy.target);

	aiAgent.speed = StatusAPI::GetFloat(status, "miaiMoveSpeed");

	float dx = transform.position.x - targetTrans->position.x;
	float dz = transform.position.z - targetTrans->position.z;

	float currentAngle = Mathf::Atan2(dz, dx);
	float radius = Mathf::Sqrt(dx * dx + dz * dz);

	float rotationSpeed = 0.01f;
	float nextAngle = currentAngle + rotationSpeed;

	float posX = targetTrans->position.x + radius * Mathf::Cos(nextAngle);
	float posZ = targetTrans->position.z + radius * Mathf::Sin(nextAngle);

	AIAgentSystem::GetInstance()->SetDestination(aiAgent, Vector3(posX, transform.position.y, posZ));
}

void EnemySystem::Approach(World& world, Enemy& enemy, AIAgent& aiAgent, Transform& transform, CharacterStatus& status)
{
	Transform* targetTrans = world.GetComponent<Transform>(enemy.target);

	aiAgent.speed = StatusAPI::GetFloat(status, "approachMoveSpeed");

	AIAgentSystem::GetInstance()->SetDestination(aiAgent, targetTrans->position);
}

void EnemySystem::JumpAttack(Entity& entity, Enemy& enemy, AIAgent& aiAgent, Transform& transform, MoveState& state, Animator& animator, World& world)
{
	const float startTime = 0.51f;
	const float endTime = 1.3f;
	const float duration = endTime - startTime;

	if (!state.isAnimed)
	{
		aiAgent.updatePosition = false;

		const MoveData& move = CharacterInfoRegistry::GetInstance()->GetMoveById(state.name, state.currentMoveId);
		const AttackParams& params = std::get<AttackParams>(move.params);

		AnimationSystem::Play(animator, params.animationName, true);

		state.isAnimed = true;
	}

	float currentTime = state.timer;

	if (currentTime < startTime)
	{
		Transform* targetTrans = world.GetComponent<Transform>(enemy.target);

		if (targetTrans)
		{
			enemy.lastTargetPos = targetTrans->position;

			Vector3 dir = enemy.lastTargetPos - transform.position;
			dir.y = 0;

			if (dir.SqrMagnitude() > 0.001f)
			{
				dir = dir.Normalized();

				Quaternion targetRot = Quaternion::LookRotation(dir);

				transform.rotation = Quaternion::Slerp(transform.rotation, targetRot, Time::GetDeltaTime() * 10.0f);
				transform.dirty = true;
			}
		}

		enemy.startJumpPos = transform.position;
	}
	else if (currentTime >= startTime && currentTime <= endTime)
	{
		float t = (currentTime - startTime) / duration;
		float easeT = t * (2.0f - t);

		transform.position = Vector3::Lerp(enemy.startJumpPos, enemy.lastTargetPos, easeT);
		transform.dirty = true;
	}
	else
	{
		transform.position = enemy.lastTargetPos;
		transform.dirty = true;
	}
}

void EnemySystem::Start(World& world)
{
}

void EnemySystem::Update(World& world)
{
	View<Enemy, AIAgent, Transform, Collider, Animator, HP, MoveState, Damageable, Attackable, LocomotionData, CharacterStatus, AIState> view(world);

	for (auto [entity, enemy, aiAgent, transform, collider, animator, hp, state, damageable, attackable, loco, status, aiState] : view)
	{
		Transform* targetTrans = world.GetComponent<Transform>(enemy.target);

		Vector3 toTarget = targetTrans->position - transform.position;
		float distance = toTarget.Magnitude();

		if (hp.isDeath)
		{
			if (animator.currentClipName != "Death")
			{
				animator.isLoop = false;
				AnimationSystem::Play(animator, "Death");
			}

			aiAgent.updatePosition = false;

			if (!animator.isPlaying)
			{
				SceneManager::ChangeScene("Title");
			}

			continue;
		}

		switch (aiState.currentStateID)
		{
		case "Miai"_h:
			Miai(world, enemy, aiAgent, transform, status);
			break;

		case "Approach"_h:
			Approach(world, enemy, aiAgent, transform, status);
			break;

		case "Recovery"_h:
			AIAgentSystem::GetInstance()->ResetAI(aiAgent);
			break;

		default:
			break;
		}

		const MoveData& currentMove = CharacterInfoRegistry::GetInstance()->GetMoveById(state.name, state.currentMoveId);

		switch (currentMove.type)
		{
		case MoveType::Idle:
			Move(world, enemy, aiAgent, transform, animator, loco);
			break;

		case MoveType::Attack:
			aiAgent.updatePosition = false;

			if (currentMove.moveId == "attack-jump"_h)
			{
				JumpAttack(entity, enemy, aiAgent, transform, state, animator, world);
			}

			break;

		case MoveType::Guard:
		case MoveType::Rolling:
			aiAgent.updatePosition = false;
			break;
		}

		if (currentMove.type == MoveType::Attack)
		{
			const AttackParams& params = std::get<AttackParams>(currentMove.params);

			if (!state.isAnimed)
			{
				AnimationSystem::Play(animator, params.animationName);

				AIAgentSystem::GetInstance()->ResetAI(aiAgent);

				state.isAnimed = true;
				aiAgent.updatePosition = false;
				loco.state = LocomotionState::Idle;
			}

			if (currentMove.duration > 0.0f && params.hitEndTime <= state.timer / currentMove.duration)
			{
				switch (state.currentMoveId)
				{
				case "attack-left"_h:
				case "attack-1-left"_h:
					StatusAPI::SetFloat(
						status,
						"AttackCoolDownTimer",
						StatusAPI::GetFloat(status, "AttackCoolDownTime")
					);
					break;

				case "attack-jump"_h:
					StatusAPI::SetFloat(
						status,
						"JumpAttackCoolDownTimer",
						StatusAPI::GetFloat(status, "JumpAttackCoolDownTime")
					);
					break;

				default:
					break;
				}

				StatusAPI::SetFloat(
					status,
					"RecoveryTimer",
					StatusAPI::GetFloat(status, "RecoveryTime")
				);
			}
		}

		StatusAPI::SetFloat(
			status,
			"AttackCoolDownTimer",
			StatusAPI::GetFloat(status, "AttackCoolDownTimer") - Time::GetDeltaTime()
		);

		StatusAPI::SetFloat(
			status,
			"JumpAttackCoolDownTimer",
			StatusAPI::GetFloat(status, "JumpAttackCoolDownTimer") - Time::GetDeltaTime()
		);

		StatusAPI::SetFloat(
			status,
			"RecoveryTimer",
			StatusAPI::GetFloat(status, "RecoveryTimer") - Time::GetDeltaTime()
		);
	}
};