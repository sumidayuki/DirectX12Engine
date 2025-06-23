#pragma once

enum class BossAttackPattern
{
	AttackA,
	AttackB,
	AttackC,
	Wait,
};

/// <summary>
/// ボスを表すタグコンポーネントです。
/// </summary>
struct Boss
{
	BossAttackPattern currentPattern = BossAttackPattern::Wait;
	BossAttackPattern previousPattern = BossAttackPattern::AttackC;
	float patternTimer = 0.0f;
	float patternDuration = 6.0f;

	float coolDownTimer = 0.0f;
	float coolDown = 1.0f;

	float damageTimer = 0.0f;
};
