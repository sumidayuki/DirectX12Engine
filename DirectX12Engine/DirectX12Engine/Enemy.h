#pragma once

enum class EnemyCondition
{
	Normal,
	angry
};

struct Enemy : IComponentData
{
	Entity target;
	EnemyCondition condition = EnemyCondition::Normal;
	Vector3 lastTargetPos = Vector3::zero;
	Vector3 startJumpPos = Vector3::zero;
	
	float recoveryTime = 10.0f;
	float recoveryTimer = 0.0f;

	float jumpAttackCoolDown = 45.0f;
	float jumpAttackCoolDownTimer = 15.0f;

	float attackCoolDown = 30.0f;
	float attackCoolDownTimer = 10.0f;

	float stateTimer = 0.0f;
};