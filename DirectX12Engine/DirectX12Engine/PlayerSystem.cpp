#include "Precompiled.h"
#include "PlayerSystem.h"
#include "PlayerTag.h"
#include "PlayerCamera.h"
#include "ScopedProfiler.h"
#include "Arrow.h"
#include "ComboInput.h"
#include "BattleCamera.h"
#include "GuardState.h"
#include "CharacterImporter.h"
#include "LocomotionUtility.h"

// ターンアニメーションの再生終了と、その後の座標確定を管理
bool PlayerSystem::ProcessTurn(World& world, Transform& transform, Animator& animator, LocomotionData& loco)
{
	if (loco.state == LocomotionState::Turning)
	{
		if (!animator.isPlaying)
		{
			transform.rotation = loco.turnTargetRot;
			transform.dirty = true;

			TransformSystem::GetInstance()->EvaluateImmediate(world, transform);

			return false;
		}
		return true;
	}
	return false;
}

// 入力に基づいたキャラクターの移動と回転を制御
void PlayerSystem::Move(World& world, Transform& transform, Input& input, Animator& anim, LocomotionData& loco, RollingState& rolling, Stamina& stamina)
{
	if (ProcessTurn(world, transform, anim, loco))
	{
		return;
	}

	Vector3 camForward = m_cameraTransform->rotation * Vector3::forward;
	Vector3 camRight = m_cameraTransform->rotation * Vector3::right;
	camForward.y = 0; camRight.y = 0;
	camForward = camForward.Normalized();
	camRight = camRight.Normalized();
	Vector3 moveDirection = camForward * input.direction.y + camRight * input.direction.x;

	std::string targetClip = "Idle";
	const float moveThreshold = 0.01f;
	
	Vector3 targetDirection = moveDirection;
	loco.currentVelocity = Vector3::Lerp(loco.currentVelocity, targetDirection, Time::GetDeltaTime() * 15.0f);

	if (input.isRolling && stamina.value >= 25.0f)
	{
		stamina.value -= 25.0f;
		stamina.timer = 0.0f;

		Vector3 rollDir = transform.rotation * Vector3::forward;
		rollDir.y = 0;
		rollDir = rollDir.Normalized();
		transform.rotation = Quaternion::LookRotation(rollDir, Vector3::up);
		rolling.direction = rollDir;
		rolling.timer = 0.0f;
		rolling.isRolling = true;
		rolling.isInvincible = false;
		loco.currentVelocity = Vector3(0, 0, 0);
		AnimationSystem::Play(anim, "Rolling");
		anim.isLoop = false;
		return;
	}

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

				transform.rotation = Quaternion::Slerp(transform.rotation, targetRotation, Time::GetDeltaTime() * 10);
				TransformSystem::GetInstance()->Translate(
					transform, inputDir * m_currentSpeed * Time::GetDeltaTime());
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

// 弓矢の生成と発射
void PlayerSystem::DrawArrow(Transform& transform, float speed, float damage, Animator& anim, World& world)
{
	Vector3 forward = transform.rotation * Vector3::forward;
	forward.y = 0;
	forward.Normalized();

	Vector3 pos = TransformSystem::GetInstance()->GetPosition(*m_bowTransform);
	Entity a = world.CreateWithModel(L"Assets/Arrow.fbx", nullptr, pos, Quaternion::LookRotation(forward));

	Projectile projectile;
	projectile.lifeTime = 1.0f;
	projectile.speed = speed;
	projectile.velocity = forward * projectile.speed;
	world.AddComponent<Projectile>(a, projectile);

	Attackable attackable;
	attackable.damage = damage;
	attackable.damageType = (anim.currentClipName == "Attack_00") ? DamageType::Normal : DamageType::Heavy;
	world.AddComponent<Attackable>(a, attackable);

	Collider collider;
	collider.type = ColliderType::Sphere;
	collider.radius = 10.0f;
	collider.isTrigger = true;
	world.AddComponent<Collider>(a, collider);

	world.AddComponent<Arrow>(a, Arrow{});
}

// 近接攻撃（脚）の当たり判定処理
void PlayerSystem::LegAttack(Transform& transform, ComboState& state, Animator& anim, Attackable& attackable, World& world)
{
	m_legAttackColl->isEnable = true;
	attackable.isAttacking = true;
	if (m_legAttackColl->info.state == CollisionState::Enter || m_legAttackColl->info.state == CollisionState::Stay)
	{
		Entity target = m_legAttackColl->info.other;
		if (world.HasComponent<Enemy>(target))
		{
			auto it = std::find(attackable.entities.begin(), attackable.entities.end(), target);
			if (it == attackable.entities.end())
			{
				Damageable* damageable = world.GetComponent<Damageable>(target);
				if (damageable)
				{
					Damage damage;
					damage.type = DamageType::Normal;
					damage.damage = 30;
					damageable->damageQueue.push(damage);
						attackable.entities.push_back(target);
				}
				state.hitConfirm = true;
			}
		}
	}
}

void PlayerSystem::Start(World& world)
{
	Entity camera = world.FindEntityOfType<PlayerCamera>();
	m_cameraTransform = world.GetComponent<Transform>(camera);
	m_playerCamera = world.GetComponent<PlayerCamera>(camera);

	m_currentSpeed = WalkSpeed;
	m_stateTimer = 0.0f;
	m_currentState = PlayerState::Move;
	m_bowTransform = nullptr;
	m_legAttackColl = nullptr;
	m_hpBar = nullptr;
}

// システムのメインループ
void PlayerSystem::Update(World& world)
{
	View<PlayerTag, Transform, Input, Animator, ComboState, Attackable, HP, Damageable, GuardState, LocomotionData, RollingState, Stamina> view(world);

	for (auto [entity, playerTag, transform, input, animator, state, attackable, hp, damageable, guard, loco, rolling, stamina] : view)
	{
		// 死亡判定
		if (hp.isDeath)
		{
			if (animator.currentClipName != "Death")
			{
				animator.isLoop = false;
				AnimationSystem::Play(animator, "Death");
			}
			if (!animator.isPlaying) SceneManager::ChangeScene("Title");
			continue;
		}

		// ガード状態の反映
		if (guard.isGuarding)
		{
			loco.currentVelocity = Vector3(0, 0, 0);

			Slider* guardBar = world.GetComponent<Slider>(UIManager::GetInstance()->GetUIObject(HashString("MainSceneUI"), HashString("PlayerGuardBar")));
			guardBar->value = guard.shieldHealth;

			continue;
		}
		else
		{
			Slider* guardBar = world.GetComponent<Slider>(UIManager::GetInstance()->GetUIObject(HashString("MainSceneUI"), HashString("PlayerGuardBar")));
			guardBar->value = guard.shieldHealth;
		}

		// 被ダメージ処理（Hitアニメーション再生）
		if (!damageable.damageQueue.empty())
		{
			Damage damage = damageable.damageQueue.front();
			if (damage.type == DamageType::Normal)
			{
				if (!rolling.isInvincible)
				{
					animator.isLoop = false;
					AnimationSystem::Play(animator, "Hit_00", true);
					hp.currentHP -= damage.damage;
					m_hpBar->value = hp.currentHP;
				}

				damageable.damageQueue.pop();
			}

			loco.state = LocomotionState::Idle;
		}
		if (animator.currentClipName == "Hit_00" && animator.isPlaying) continue;

		if (!m_bowTransform)
		{
			Entity right = world.CreateEntity();
			BoneSocket socket2;
			socket2.targetEntity = entity;
			socket2.targetBoneName = "mixamorig:Left_arch2";
			world.AddComponent<BoneSocket>(right, socket2);

			m_bowTransform = world.GetComponent<Transform>(right);
		}

		if (!m_legAttackColl)
		{
			Entity leg = world.CreateEntity();
			Collider legColl;
			legColl.type = ColliderType::Sphere;
			legColl.radius = 30.0f;
			legColl.isTrigger = true;
			legColl.collisionMask = Layers::Enemy;
			BoneSocket socket;
			socket.targetEntity = entity;
			socket.targetBoneName = "mixamorig:RightToeBase";
			world.AddComponent<BoneSocket>(leg, socket);
			world.AddComponent<Attackable>(leg, Attackable{});
			m_legAttackColl = world.AddComponent<Collider>(leg, legColl);
			m_legAttackColl->isEnable = false;
		}

		if (!m_hpBar)
		{
			m_hpBar = world.GetComponent<Slider>(UIManager::GetInstance()->GetUIObject(HashString("MainSceneUI"), HashString("PlayerHPBar")));
			m_hpBar->maxValue = hp.maxHP;
			m_hpBar->minValue = 0;
			m_hpBar->value = hp.maxHP;

			Slider* guardBar = world.GetComponent<Slider>(UIManager::GetInstance()->GetUIObject(HashString("MainSceneUI"), HashString("PlayerGuardBar")));
			guardBar->maxValue = guard.shieldMaxHealth;
			guardBar->minValue = 0;
			guardBar->value = guard.shieldHealth;
		}

		if (rolling.isRolling)
		{
			rolling.timer += Time::GetDeltaTime();
			float progress = rolling.timer / rolling.duration;

			rolling.isInvincible = (progress >= rolling.invincibleStart && progress <= rolling.invincibleEnd);
			
			float easedSpeed = rolling.speed * Mathf::Max(0.0f, 1.0f - progress * progress);
			
			TransformSystem::GetInstance()->Translate(transform, rolling.direction * easedSpeed * Time::GetDeltaTime());
			
			if (progress >= 1.0f || !animator.isPlaying)
			{
				rolling.isRolling = false;
				rolling.isInvincible = false;
				loco.state = LocomotionState::Idle;
			}
			continue;
		}

		// 移動または攻撃への分岐
		if (state.currentMoveId == 0)
		{
			Move(world, transform, input, animator, loco, rolling, stamina);
			continue;
		}

		const ComboMove& currentMove = CharacterImporter::GetInstance()->GetMoveById(state.name, state.currentMoveId);

		if (!currentMove.isAttack)
		{
			Move(world, transform, input, animator, loco, rolling, stamina);
			continue;
		}

		loco.currentVelocity = Vector3(0, 0, 0);

		// コンボ攻撃中の回転・攻撃実行
		animator.isLoop = false;

		// 敵の情報を取得
		if (!m_gameManager)
		{
			m_gameManager = world.GetSystem<GameManagerSystem>();
		}

		Entity enemy = m_gameManager->GetEnemy();
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

				// 技の開始直後（timerが非常に小さい時）は瞬時に向かせる
				if (state.timer < 0.05f)
				{
					transform.rotation = targetRot;
				}
				else
				{
					// それ以外は高速で補間（AIAgentの回転より速い値を設定）
					transform.rotation = Quaternion::Slerp(transform.rotation, targetRot, Time::GetDeltaTime() * 20.0f);
				}
			}
		}

		if (!state.isAnimed)
		{
			AnimationSystem::Play(animator, currentMove.animationName, true);
			state.isAnimed = true;
		}

		if (state.canHit && !state.hitConfirm)
		{
			bool canFire = true;

			if (enemyTransform)
			{
				Vector3 forward = transform.rotation * Vector3::forward;
				Vector3 targetDir = (enemyTransform->position - transform.position);
				targetDir.y = 0;
				targetDir.Normalized();

				transform.dirty = true;

				// 敵との角度差をドット積でチェック（0.95 は約18度以内）
				float dot = Vector3::Dot(forward.Normalized(), targetDir.Normalized());
				if (dot < 0.95f)
				{
					canFire = false; // まだ向いていないので撃たない
				}
			}

			if (canFire)
			{
				switch (currentMove.moveId)
				{
				case "attack-normal-1"_h:
				case "attack-normal-2"_h:
				case "attack-normal-3"_h:
					DrawArrow(transform, 2000, currentMove.damage, animator, world);
					state.hitConfirm = true;
					break;

				case "attack-guard"_h:
					LegAttack(transform, state, animator, attackable, world);
					break;

				default:
					break;
				}
			}
		}
		else
		{
			m_legAttackColl->isEnable = false;
			attackable.isAttacking = false;
			attackable.entities.clear();
		}
	}
}