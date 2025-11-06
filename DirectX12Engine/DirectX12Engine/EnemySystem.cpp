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

	if (enemy.stateTimer < animTime)
	{
		if (animator.currentClipName != "PunchLeft")
		{
			AnimationSystem::Play(animator, "PunchLeft");
		}
	}
	else
	{
		if (animator.currentClipName != "PunchRight")
		{
			AnimationSystem::Play(animator, "PunchRight");
		}

		if (!animator.isPlaying)
		{
			enemy.state = EnemyState::Idle;
			animator.isLoop = true;
			enemy.stateTimer = 0.0f;
		}
	}
}

void EnemySystem::Start(ComponentManager& cm, World& world)
{
}

void EnemySystem::Update(ComponentManager& cm, World& world)
{
	View<Enemy, AIAgent, Transform, Animator> view(cm);

	for (auto [entity, enemy, aiAgent, transform, animator] : view)
	{
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