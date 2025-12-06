#include "Precompiled.h"

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

void EnemySystem::Attack(Entity& entity, Enemy& enemy, Transform& transform, Animator& animator)
{
	animator.isLoop = false;
	const float animTime = animator.clips["PunchLeft"]->GetTotalTime();

	// 現在のステートタイマーがアニメーション時間より小さいなら左手の攻撃
	if (enemy.stateTimer < animTime)
	{
		// 現在のアニメーションクリップが PunchLeft ではないなら
		if (animator.currentClipName != "PunchLeft")
		{
			// PunchLeft を再生
			AnimationSystem::Play(animator, "PunchLeft");
		}

		// アニメーションタイムの時間によってコライダーをオンオフして攻撃タイミングを設定。
		if (enemy.stateTimer >= animTime * 0.1f && enemy.stateTimer <= animTime * 0.5f)
		{
			// コライダーを有効化
			m_leftHandColl->isEnable = true;
		}
		else
		{
			// コライダーを無効化
			m_leftHandColl->isEnable = false;
		}
	}
	// 大きいなら右手の攻撃
	else
	{
		// 現在のアニメーションクリップが PunchRight ではないなら
		if (animator.currentClipName != "PunchRight")
		{
			AnimationSystem::Play(animator, "PunchRight");
		}

		// アニメーションタイムの時間によってコライダーをオンオフして攻撃タイミングを設定。
		if (enemy.stateTimer >= animTime + (animTime * 0.1f) && enemy.stateTimer <= animTime + (animTime * 0.5f))
		{
			m_rightHandColl->isEnable = true;
		}
		else
		{
			m_rightHandColl->isEnable = false;
		}

		// アニメーションの再生が終了していたら
		if (!animator.isPlaying)
		{
			// ステートを切り替える
			enemy.state = EnemyState::Idle;
			animator.isLoop = true;
			enemy.stateTimer = 0.0f;
		}
	}
}

void EnemySystem::Start(World& world)
{
}

void EnemySystem::Update(World& world)
{
	View<Enemy, AIAgent, Transform, Animator, HP> view(world);

	for (auto [entity, enemy, aiAgent, transform, animator, hp] : view)
	{
		// コライダーが設定されていないなら
		if (!m_leftHandColl && !m_rightHandColl)
		{
			// 左手のコライダーを設定
			Entity left = world.CreateEntity();
			SphereCollider leftColl;
			leftColl.radius = 30.0f;
			BoneSocket socket;
			socket.targetEntity = entity;
			socket.targetBoneName = "mixamorig:LeftHand";
			world.AddComponent<BoneSocket>(left, socket);
			m_leftHandColl = world.AddComponent<SphereCollider>(left, leftColl);
			m_leftHandColl->isEnable = false;

			// 右手のコライダーを設定
			Entity right = world.CreateEntity();
			SphereCollider rightColl;
			rightColl.radius = 30.0f;
			BoneSocket socket2;
			socket2.targetEntity = entity;
			socket2.targetBoneName = "mixamorig:RightHand";
			world.AddComponent<BoneSocket>(right, socket2);
			m_rightHandColl = world.AddComponent<SphereCollider>(right, rightColl);
			m_rightHandColl->isEnable = false;
		}

		if (hp.isDeath)
		{
			if (animator.currentClipName != "Death")
			{
				animator.isLoop = false;
				aiAgent.speed = 0;
				AnimationSystem::Play(animator, "Death");
			}

			if (!animator.isPlaying)
			{
				world.DestroyEntity(entity);
			}

			continue;
		}

		enemy.stateTimer += Time::GetDeltaTime();

		switch (enemy.state)
		{
		case EnemyState::Idle:
			Idle(entity, enemy, transform, animator);
			break;

		case EnemyState::Chase:
			Chase(entity, enemy, aiAgent, transform, animator, world);
			break;

		case EnemyState::Attack:
			Attack(entity, enemy, transform, animator);
			break;

		case EnemyState::Retreat:
			break;
		}

	}
}