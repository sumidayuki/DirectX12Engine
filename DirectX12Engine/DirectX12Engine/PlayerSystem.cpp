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

void PlayerSystem::Move(Transform& transform, Input& input, Animator& animator)
{
	// カメラのTransformがなければ処理しない
	if (!m_cameraTransform) return;

	// カメラの向きを基準とした前方と右方向を取得
	Vector3 camForward = m_cameraTransform->rotation * Vector3::forward;
	Vector3 camRight = m_cameraTransform->rotation * Vector3::right;

	// Y軸の傾きを無視して、水平なベクトルにする
	camForward.y = 0;
	camRight.y = 0;
	camForward.Normalized();
	camRight.Normalized();

	// カメラの向きと入力から、ワールド座標系での移動方向を決定
	Vector3 moveDirection = camForward * input.direction.y + camRight * input.direction.x;

	// アニメーションクリップの決定
	std::string targetClip = "Idle";

	// 移動入力があるかどうかの閾値
	const float moveThreshold = 0.01f;

	if (moveDirection.SqrMagnitude() > moveThreshold)
	{
		moveDirection = moveDirection.Normalized(); // 方向を正規化

		// 移動速度を決定
		m_currentSpeed = input.dash ? RunSpeed : WalkSpeed;

		// アニメーションを決定 (常に前進アニメーションを使用)
		targetClip = (m_currentSpeed == RunSpeed) ? "Run_Forward" : "Walk_Forward";

		// プレイヤーの向きを、移動方向に滑らかに向ける
		Quaternion targetRotation = Quaternion::LookRotation(moveDirection, Vector3::up);
		const float rotationSpeed = 45.0f; // 回転の速さ (値が大きいほど速い)
		transform.rotation = Quaternion::Slerp(transform.rotation, targetRotation, Time::GetDeltaTime() * rotationSpeed);

		// プレイヤーを移動させる
		TransformSystem::GetInstance()->Translate(transform, moveDirection * m_currentSpeed * Time::GetDeltaTime());
	}

	// アニメーションの切り替え
	if (animator.currentClipName != targetClip)
	{
		AnimationSystem::Play(animator, targetClip);
	}
}

void PlayerSystem::DrawArrow(Transform& transform, float damage, Animator& anim, World& world)
{
	Vector3 forward = transform.rotation * Vector3::forward;
	forward.y = 0;
	forward.Normalized();

	Vector3 pos = TransformSystem::GetInstance()->GetPosition(*m_bowTransform);

	Entity a = world.CreateWithModel(L"Assets/Arrow.fbx", nullptr, pos, Quaternion::LookRotation(forward));

	Projectile projectile;
	projectile.lifeTime = 1.0f;
	projectile.speed = 2000.0f;
	world.AddComponent<Projectile>(a, projectile);

	Attackable attackable;
	attackable.damage = damage;
	if (anim.currentClipName == "Attack_00")
	{
		attackable.damageType = DamageType::Normal;
	}
	else
	{
		attackable.damageType = DamageType::Heavy;
	}
	world.AddComponent<Attackable>(a, attackable);
	Collider collider;
	collider.type = ColliderType::Sphere;
	collider.radius = 10.0f;
	collider.isTrigger = true;
	world.AddComponent<Collider>(a, collider);

	world.AddComponent<Arrow>(a, Arrow{});
}

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
	Entity camera = world.FindEntityOfType<BattleCamera>();
	m_cameraTransform = world.GetComponent<Transform>(camera);

	// 初期値
	m_currentSpeed = WalkSpeed;
	m_stateTimer = 0.0f;
	m_currentState = PlayerState::Move;
}

void PlayerSystem::Update(World& world)
{
	View<PlayerTag, Transform, Input, Animator, ComboState, Attackable, HP, Damageable, GuardState> view(world);

	for (auto [entity, playerTag, transform, input, animator, state, attackable, hp, damageable, guard] : view)
	{
		if (hp.isDeath)
		{
			if (animator.currentClipName != "Death")
			{
				animator.isLoop = false;
				AnimationSystem::Play(animator, "Death");
			}
			continue;
		}

		// ガード中はガードシステムに処理を任せる
		guard.isGuarding = input.isGuard;

		if (input.isGuard)
		{
			continue;
		}

		if (!damageable.damageQueue.empty())
		{
			for (int i = 0; i < damageable.damageQueue.size(); i++)
			{
				Damage damage = damageable.damageQueue.front();

				switch (damage.type)
				{
				case DamageType::Normal:
					animator.isLoop = false;
					AnimationSystem::Play(animator, "Hit_00", true);
					hp.currentHP -= damage.damage;
					damageable.damageQueue.pop();
					break;

				default:
					break;
				}
			}
		}

		if (animator.currentClipName == "Hit_00" && animator.isPlaying)
		{
			continue;
		}

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
			BoneSocket socket;
			socket.targetEntity = entity;
			socket.targetBoneName = "mixamorig:RightToeBase";
			world.AddComponent<BoneSocket>(leg, socket);
			world.AddComponent<Attackable>(leg, Attackable{});
			m_legAttackColl = world.AddComponent<Collider>(leg, legColl);
			m_legAttackColl->isEnable = false;
		}

		if (state.currentMoveId == 0)
		{
			animator.isLoop = true;

			Move(transform, input, animator);

			continue;
		}

		animator.isLoop = false;

		const ComboMove& currentMove = CharacterImporter::GetInstance()->GetMoveById(state.name, state.currentMoveId);

		// 敵の情報を取得
		static GameManagerSystem* gameManager;
		if (!gameManager)
			gameManager = world.GetSystem<GameManagerSystem>();
		Entity enemy = gameManager->GetEnemy();
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

				switch (currentMove.attackType)
				{
				case 1:
					DrawArrow(transform, currentMove.damage, animator, world);
					state.hitConfirm = true;
					break;
				case 2:
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
