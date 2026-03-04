#pragma once

enum class EnemyState
{
	Idle,
	Chase,
	Attack,
	Retreat,
	Roaring,
	Death
};

enum class EnemyCondition
{
	Normal,
	angry
};

struct Enemy : IComponentData
{
	Entity target;
	EnemyState state = EnemyState::Idle;
	EnemyCondition condition = EnemyCondition::Normal;
	Vector3 lastTargetPos = Vector3::zero;
	Vector3 startJumpPos = Vector3::zero;
	float attackRange = 50.0f;
	float attackCoolDown = 3.0f;
	float stateTimer = 0.0f;
};