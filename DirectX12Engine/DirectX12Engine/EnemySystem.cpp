#include "Precompiled.h"
#include "PlayerTag.h"
#include "CharacterImporter.h"

void EnemySystem::Move(Enemy& enemy, AIAgent& aiAgent, Transform& transform, Animator& animator, World& world)
{
	aiAgent.updatePosition = true;
	aiAgent.updateRotation = false;

	Transform* targetTrans = world.GetComponent<Transform>(enemy.target);
	Vector3 targetPos = targetTrans->position;

	Vector3 dir = (targetPos - transform.position).Normalized();
	dir.y = 0; // Y軸（上下）の回転は無視

	if (dir.SqrMagnitude() > 0.001f)
	{
		Quaternion targetRot = Quaternion::LookRotation(dir);
		// 補間（Slerp）を使って滑らかに向かせる
		transform.rotation = Quaternion::Slerp(transform.rotation, targetRot, Time::GetDeltaTime() * 10.0f);
		transform.dirty = true;
	}

	// AIAgentの速度に基づいてアニメーションを切り替える
	float speed = aiAgent.velocity.Magnitude();

	if (speed > 10.0f) // 移動中
	{
		if (animator.currentClipName != "Walk") 
		{
			AnimationSystem::Play(animator, "Walk");
			animator.isLoop = true;
		}
	}
	else // 待機中（ここが Idle 状態）
	{
		if (animator.currentClipName != "Idle") 
		{
			AnimationSystem::Play(animator, "Idle");
			animator.isLoop = true;
		}
	}
}

void EnemySystem::JampAttack(Entity& entity, Enemy& enemy, AIAgent& aiAgent, Transform& transform, ComboState& state, Animator& animator, World& world)
{
	// 設定値（アニメーションに合わせる）
	const float startTime = 0.51f; // 移動開始秒数
	const float endTime = 1.60f; // ぴったり着く秒数
	const float duration = endTime - startTime;

	// 1. 攻撃開始の最初のフレーム
	if (!state.isAnimed)
	{
		aiAgent.updatePosition = false;

		// ターゲットの現在地を「最終目的地」としてロック
		Transform* targetTrans = world.GetComponent<Transform>(enemy.target);
		if (targetTrans) 
		{
			enemy.lastTargetPos = targetTrans->position;
		}

		enemy.startJumpPos = transform.position;

		const auto& move = CharacterImporter::GetInstance()->GetMoveById(state.name, state.currentMoveId);
		AnimationSystem::Play(animator, move.animationName);
		state.isAnimed = true;
	}

	float currentTime = state.timer;

	// 2. 移動中
	if (currentTime >= startTime && currentTime <= endTime)
	{
		float t = (currentTime - startTime) / duration;

		transform.position = Vector3::Lerp(enemy.startJumpPos, enemy.lastTargetPos, t);
		transform.dirty = true;
	}
	// 3. 移動時間を過ぎた場合（完全に目的地に固定）
	else if (currentTime > endTime)
	{
		transform.position = enemy.lastTargetPos;
		transform.dirty = true;
	}
}

bool EnemySystem::ProcessCollision(World& world, Collider* coll, ComboState& state, Attackable& attackable)
{
	if (coll->info.state == CollisionState::Enter || coll->info.state == CollisionState::Stay)
	{
		if (world.GetComponent<PlayerTag>(coll->info.other))
		{
			// 重複ヒットチェック
			for (auto targetEntity : attackable.entities) 
			{
				if (targetEntity == coll->info.other)
				{
					return false;
				}
			}

			const ComboMove& move = CharacterImporter::GetInstance()->GetMoveById(state.name, state.currentMoveId);
			Damageable* damageable = world.GetComponent<Damageable>(coll->info.other);

			if (damageable) 
			{
				Damage dmg;
				dmg.damage = move.damage;
				dmg.type = DamageType::Normal;
				damageable->damageQueue.push(dmg);
				attackable.entities.push_back(coll->info.other);
				return true;
			}
		}
	}
	return false;
}

void EnemySystem::Start(World& world)
{
	m_hitBox = nullptr;
	m_leftHandColl = nullptr;
	m_rightHandColl = nullptr;
	m_jumpAttackColl = nullptr;
}

void EnemySystem::Update(World& world)
{
	View<Enemy, AIAgent, Transform, Collider, Animator, HP, ComboState, Damageable, Attackable> view(world);

	for (auto [entity, enemy, aiAgent, transform, collider, animator, hp, state, damageable, attackable] : view)
	{
		// コライダーが設定されていないなら
		if (!m_leftHandColl && !m_rightHandColl)
		{
			// 左手のコライダーを設定
			Entity left = world.CreateEntity();
			Collider leftColl;
			leftColl.type = ColliderType::Sphere;
			leftColl.radius = 30.0f;
			leftColl.isTrigger = true;
			BoneSocket socket;
			socket.targetEntity = entity;
			socket.targetBoneName = "mixamorig:LeftHand";
			world.AddComponent<BoneSocket>(left, socket);
			world.AddComponent<Attackable>(left, Attackable{});
			m_leftHandColl = world.AddComponent<Collider>(left, leftColl);
			m_leftHandColl->isEnable = false;

			// 右手のコライダーを設定
			Entity right = world.CreateEntity();
			Collider rightColl;
			rightColl.type = ColliderType::Sphere;
			rightColl.radius = 30.0f;
			rightColl.isTrigger = true;
			BoneSocket socket2;
			socket2.targetEntity = entity;
			socket2.targetBoneName = "mixamorig:RightHand";
			world.AddComponent<BoneSocket>(right, socket2);
			world.AddComponent<Attackable>(right, Attackable{});
			m_rightHandColl = world.AddComponent<Collider>(right, rightColl);
			m_rightHandColl->isEnable = false;

			Entity jump = world.CreateEntity();
			Collider jumpColl;
			jumpColl.type = ColliderType::Sphere;
			jumpColl.radius = 80.0f;
			jumpColl.isTrigger = true;
			BoneSocket socket3;
			socket3.targetEntity = entity;
			socket3.targetBoneName = "mixamorig:Hips";
			world.AddComponent<BoneSocket>(jump, socket3);
			world.AddComponent<Attackable>(jump, Attackable{});
			m_jumpAttackColl = world.AddComponent<Collider>(jump, jumpColl);
			m_jumpAttackColl->isEnable = false;
		}

		// 死亡・被弾処理は最優先（PlayerSystemのガード等の割り込みに近い考え方）
		if (hp.isDeath)
		{
			if (animator.currentClipName != "Death")
			{
				AnimationSystem::Play(animator, "Death");
			}

			aiAgent.updatePosition = false;
			
			if (!animator.isPlaying)
			{
				SceneManager::ChangeScene("Title");
			}
			continue;
		}

		// 被弾による硬直とコンボ強制終了
		if (!damageable.damageQueue.empty()) 
		{
			while (!damageable.damageQueue.empty())
			{

				if (state.currentMoveId != 3)
				{
					state.currentMoveId = 0; // コンボ中断
					animator.isLoop = false;
					AnimationSystem::Play(animator, "Hit_00", true);
					hp.currentHP -= damageable.damageQueue.front().damage;
				}
				damageable.damageQueue.pop();
			}
		}

		if (animator.currentClipName == "Hit_00" && animator.isPlaying) 
		{
			aiAgent.updatePosition = false;
			continue;
		}

		switch (state.currentMoveId)
		{
		case 0:
			Move(enemy, aiAgent, transform, animator, world);
			break;
		case 3:
			JampAttack(entity, enemy, aiAgent, transform, state, animator, world);
			break;

		default:
			aiAgent.updatePosition = false;
			break;
		}

		if (state.currentMoveId != 0)
		{
			// アニメーション再生（一度だけ）
			if (!state.isAnimed) 
			{
				const auto& move = CharacterImporter::GetInstance()->GetMoveById(state.name, state.currentMoveId);
				AnimationSystem::Play(animator, move.animationName);
				state.isAnimed = true;
				aiAgent.updatePosition = false; // 攻撃中は移動停止
			}

			// ヒット判定期間（canHit）の処理
			if (state.canHit && !state.hitConfirm)
			{
				if (state.currentMoveId == 1 || state.currentMoveId == 3)
				{
					m_leftHandColl->isEnable = true;
				}
				if (state.currentMoveId == 2 || state.currentMoveId == 3)
				{
					m_rightHandColl->isEnable = true;
				}
				if (state.currentMoveId == 3)
				{
					m_jumpAttackColl->isEnable = true;
				}

				// 衝突検知
				bool hitSomething = false;
				if (m_leftHandColl->isEnable && ProcessCollision(world, m_leftHandColl, state, attackable))
				{
					hitSomething = true;
				}
				if (m_rightHandColl->isEnable && ProcessCollision(world, m_rightHandColl, state, attackable))
				{
					hitSomething = true;
				}
				if (m_jumpAttackColl->isEnable && ProcessCollision(world, m_jumpAttackColl, state, attackable))
				{
					hitSomething = true;
				}

				if (hitSomething)
				{
					state.hitConfirm = true;
				}
			}
			else
			{
				m_leftHandColl->isEnable = false;
				m_rightHandColl->isEnable = false;
				m_jumpAttackColl->isEnable = false;
				attackable.entities.clear();
				attackable.isAttacking = false;
			}
		}
		else
		{
			// MoveId == 0 (Idle/Move中) は確実にコライダーを消す
			m_leftHandColl->isEnable = false;
			m_rightHandColl->isEnable = false;
			m_jumpAttackColl->isEnable = false;
			attackable.entities.clear();
			attackable.isAttacking = false;
		}
	}
};