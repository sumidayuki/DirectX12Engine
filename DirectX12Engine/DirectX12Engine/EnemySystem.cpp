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
			TransformSystem::GetInstance()->EvaluateImmediate(world, transform);

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
		if (charForward.SqrMagnitude() > 0.001f) charForward = charForward.Normalized();

		Vector3 moveDir = Vector3(aiAgent.velocity.x, 0, aiAgent.velocity.z).Normalized();
		float angle = LocomotionUtility::CalculateMoveAngle(charForward, moveDir);
		std::string targetClip = LocomotionUtility::SelectDirectionalClip(
			angle, "Walk", "Walk_Backward", "Walk_Right", "Walk_Left");
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

			// 現在の正面方向とターゲット方向の角度差を計算
			Vector3 charForward = transform.rotation * Vector3::forward;
			charForward.y = 0;
			if (charForward.SqrMagnitude() > 0.001f) charForward = charForward.Normalized();

			float angleToTarget = LocomotionUtility::CalculateMoveAngle(charForward, dir);

			// 視界外(80度以上)に行ったらターンする。80度にすることで、真後ろ(180度)の時に90度ターンが2回連続で入りやすくなります。
			if (Mathf::Abs(angleToTarget) > 80.0f)
			{
				std::string turnClip = (angleToTarget > 0) ? "Turn_Left_90" : "Turn_Right_90";

				if (animator.clips.count(turnClip) > 0)
				{
					if (loco.state != LocomotionState::Turning)
					{
						// 90度だけ回転した状態を目標とする
						Quaternion rot90 = Quaternion::AngleAxis((angleToTarget > 0) ? 90.0f : -90.0f, Vector3::up);
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
	m_isInvincivle = true;
	Transform* targetTrans = world.GetComponent<Transform>(enemy.target);
	aiAgent.speed = StatusAPI::GetFloat(status, "approachMoveSpeed");
	AIAgentSystem::GetInstance()->SetDestination(aiAgent, targetTrans->position);
}

void EnemySystem::JumpAttack(Entity& entity, Enemy& enemy, AIAgent& aiAgent, Transform& transform, MoveState& state, Animator& animator, World& world)
{
	// 設定値
	const float startTime = 0.51f; // 移動開始秒
	const float endTime = 1.3f;
	const float duration = endTime - startTime;

	// 攻撃開始の最初のフレーム
	if (!state.isAnimed)
	{
		aiAgent.updatePosition = false;

		const MoveData& move = CharacterInfoRegistry::GetInstance()->GetMoveById(state.name, state.currentMoveId);
		const AttackParams& params = std::get<AttackParams>(move.params);
		AnimationSystem::Play(animator, params.animationName, true);
		state.isAnimed = true;
		m_isInvincivle = true;
	}

	float currentTime = state.timer;

	// ジャンプ直前までターゲットの位置を追いかけ、向きも更新し続ける
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
				transform.rotation = Quaternion::Slerp(transform.rotation, targetRot, Time::GetDeltaTime() * 10); // ターゲットの方を向く
				transform.dirty = true;
			}
		}
		enemy.startJumpPos = transform.position;
	}
	// 移動中
	else if (currentTime >= startTime && currentTime <= endTime)
	{
		float t = (currentTime - startTime) / duration;

		// イージング
		float easeT = t * (2.0f - t);

		transform.position = Vector3::Lerp(enemy.startJumpPos, enemy.lastTargetPos, easeT);
		transform.dirty = true;
	}
	// 移動時間を過ぎた場合（完全に目的地に固定）
	else if (currentTime > endTime)
	{
		transform.position = enemy.lastTargetPos;
		transform.dirty = true;
	}
}

void EnemySystem::Start(World& world)
{
	m_isInvincivle = false;
	m_hpBar = nullptr;
}

void EnemySystem::Update(World& world)
{
	View<Enemy, AIAgent, Transform, Collider, Animator, HP, MoveState, Damageable, Attackable, LocomotionData, CharacterStatus, AIState> view(world);

	for (auto [entity, enemy, aiAgent, transform, collider, animator, hp, state, damageable, attackable, loco, status, aiState] : view)
	{
		if (!m_hpBar)
		{
			m_hpBar = world.GetComponent<Slider>(UIManager::GetInstance()->GetUIObject(HashString("MainSceneUI"), HashString("EnemyHPBar")));
			m_hpBar->maxValue = hp.maxHP;
			m_hpBar->minValue = 0;
			m_hpBar->value = hp.maxHP;
		}

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

		// ダメージ処理
		if (!damageable.damageQueue.empty())
		{
			while (!damageable.damageQueue.empty())
			{

				hp.currentHP -= damageable.damageQueue.front().damage;
				m_hpBar->value = hp.currentHP;

				damageable.damageQueue.pop();
			}
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

		switch (state.currentMoveId)
		{
		case 0:
			Move(world, enemy, aiAgent, transform, animator, loco);
			break;
		case "attack-jump"_h:
			JumpAttack(entity, enemy, aiAgent, transform, state, animator, world);
			break;

		default:
			aiAgent.updatePosition = false;
			break;
		}

		if (state.currentMoveId != 0)
		{
			const MoveData& move = CharacterInfoRegistry::GetInstance()->GetMoveById(state.name, state.currentMoveId);
			const AttackParams& params = std::get<AttackParams>(move.params);

			// アニメーション再生（一度だけ）
			if (!state.isAnimed)
			{
				AnimationSystem::Play(animator, params.animationName);
				AIAgentSystem::GetInstance()->ResetAI(aiAgent);
				state.isAnimed = true;
				aiAgent.updatePosition = false; // 攻撃中は移動停止
				loco.state = LocomotionState::Idle;
			}

			if (params.hitEndTime <= state.timer / move.duration)
			{
				switch (state.currentMoveId)
				{
				case "attack-left"_h:
					StatusAPI::SetFloat(status, "AttackCoolDownTimer", StatusAPI::GetFloat(status, "AttackCoolDownTime"));
					break;

				case "attack-jump"_h:
					StatusAPI::SetFloat(status, "JumpAttackCoolDownTimer", StatusAPI::GetFloat(status, "JumpAttackCoolDownTime"));
					break;

				default:
					break;
				}

				StatusAPI::SetFloat(status, "RecoveryTimer", StatusAPI::GetFloat(status, "RecoveryTime"));
			}
		}

		StatusAPI::SetFloat(status, "AttackCoolDownTimer", StatusAPI::GetFloat(status, "AttackCoolDownTimer") - Time::GetDeltaTime());
		StatusAPI::SetFloat(status, "JumpAttackCoolDownTimer", StatusAPI::GetFloat(status, "JumpAttackCoolDownTimer") - Time::GetDeltaTime());
		StatusAPI::SetFloat(status, "RecoveryTimer", StatusAPI::GetFloat(status, "RecoveryTimer") - Time::GetDeltaTime());
	}
};