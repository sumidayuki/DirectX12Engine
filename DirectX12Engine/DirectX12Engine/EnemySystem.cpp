#include "Precompiled.h"
#include "PlayerTag.h"
#include "CharacterImporter.h"

float EnemySystem::GetTargetDistance(const Vector3 enemyPos, const Vector3 targetpos) const
{
	return (targetpos - enemyPos).SqrMagnitude();
}

void EnemySystem::Idle(Entity& entity, Enemy& enemy, Transform& transform, Animator& animator)
{
	if (animator.currentClipName != "Idle")
	{
		AnimationSystem::Play(animator, "Idle");
	}

	if (enemy.stateTimer >= 3.0f)
	{
		enemy.state = EnemyState::Chase;
		enemy.stateTimer = 0.0f;
	}
}

void EnemySystem::Chase(Entity& entity, Enemy& enemy, AIAgent& aiAgent, Transform& transform, Animator& animator, World& world)
{
	if (animator.currentClipName != "Walk")
	{
		animator.isLoop = true;
		aiAgent.updatePosition = true;
		AnimationSystem::Play(animator, "Walk");
	}

	AIAgentSystem::GetInstance()->SetDestination(aiAgent, world.GetComponent<Transform>(enemy.target)->position);

	if (GetTargetDistance(transform.position, world.GetComponent<Transform>(enemy.target)->position) < 175.0f * 175.0f)
	{
		AIAgentSystem::GetInstance()->ResetAI(aiAgent);
		enemy.state = EnemyState::Attack;
		enemy.stateTimer = 0.0f;
	}
}

void EnemySystem::Start(World& world)
{
}

void EnemySystem::Update(World& world)
{
	View<Enemy, AIAgent, Transform, Animator, HP, ComboState, Damageable, Attackable> view(world);

	for (auto [entity, enemy, aiAgent, transform, animator, hp, state, damageable, attackable] : view)
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
				world.DestroyEntity(entity);
			}
			continue;
		}

		// 被弾による硬直とコンボ強制終了
		if (!damageable.damageQueue.empty()) 
		{
			state.currentMoveId = 0; // コンボ中断


			while (!damageable.damageQueue.empty())
			{
				animator.isLoop = false;
				AnimationSystem::Play(animator, "Hit_00", true);
				hp.currentHP -= damageable.damageQueue.front().damage;
				damageable.damageQueue.pop();
			}
		}
		if (animator.currentClipName == "Hit_00" && animator.isPlaying) 
		{
			aiAgent.updatePosition = false;
			continue;
		}

        // 1. 攻撃中でなければ何もしない（またはコライダーを消す）
        if (state.currentMoveId == 0) 
		{
            m_leftHandColl->isEnable = false;
            m_rightHandColl->isEnable = false;
            attackable.entities.clear();

			aiAgent.updatePosition = true;

			Transform* targetTrans = world.GetComponent<Transform>(enemy.target);
			Vector3 targetPos = targetTrans->position;

			Vector3 dir = (targetPos - transform.position).Normalized();
			dir.y = 0; // Y軸（上下）の回転は無視

			if (dir.SqrMagnitude() > 0.001f)
			{
				Quaternion targetRot = Quaternion::LookRotation(dir);
				// 補間（Slerp）を使って滑らかに向かせる
				transform.rotation = Quaternion::Slerp(transform.rotation, targetRot, Time::GetDeltaTime() * 10.0f);
			}

			// AIAgentの速度に基づいてアニメーションを切り替える
			float speed = aiAgent.velocity.Magnitude();

			if (speed > 10.0f) // 移動中
			{
				if (animator.currentClipName != "Walk") {
					AnimationSystem::Play(animator, "Walk");
					animator.isLoop = true;
				}
			}
			else // 待機中（ここが Idle 状態）
			{
				if (animator.currentClipName != "Idle") {
					AnimationSystem::Play(animator, "Idle");
					animator.isLoop = true;
					// 次の攻撃に備えて情報をリセット
					attackable.entities.clear();
					attackable.isAttacking = false;
				}
			}

            continue;
        }

        // 2. 判定の有効化 (ComboSystemが計算した state.canHit を使う)
        // 提示コードの「if (enemy.stateTimer >= animTime * 0.1f...)」に相当
        if (state.canHit)
        {
            // MoveID 1なら左、2なら右を出す
            if (state.currentMoveId == 1) m_leftHandColl->isEnable = true;
            else if (state.currentMoveId == 2) m_rightHandColl->isEnable = true;

            attackable.isAttacking = true;



            // 3. ダメージ処理 (提示コードの重複ヒット防止ロジックを流用)
            // ※本来はCollisionSystemで行うのが理想的ですが、提示コードに合わせる場合：
            Collider* currentColl = (state.currentMoveId == 1) ? m_leftHandColl : m_rightHandColl;
            if (currentColl->info.state == CollisionState::Enter || currentColl->info.state == CollisionState::Stay)
            {
                Entity target = currentColl->info.other;
                if (world.HasComponent<PlayerTag>(target))
                {
                    auto it = std::find(attackable.entities.begin(), attackable.entities.end(), target);
                    if (it == attackable.entities.end())
                    {
                        if (auto* dmgable = world.GetComponent<Damageable>(target))
                        {
                            Damage dmg; dmg.damage = 10;
                            dmgable->damageQueue.push(dmg);
                            attackable.entities.push_back(target);
                        }
                    }
                }
            }
        }
        else
        {
            // 判定時間外ならリセット
            m_leftHandColl->isEnable = false;
            m_rightHandColl->isEnable = false;
            attackable.entities.clear(); // 次の技のためにリストを空にする
        }

        // 4. アニメーション再生 (一度だけ実行)
        if (!state.isAnimed) {
            const auto& move = CharacterImporter::GetInstance()->GetMoveById(state.name, state.currentMoveId);
            AnimationSystem::Play(animator, move.animationName);
            state.isAnimed = true;
        }

	}
}