#pragma once
#include "TransformSystem.h"

/// <summary>
/// クラス名：BossSystem
/// 概要：
/// </summary>
class BossSystem : public System
{
private:
	TransformSystem* m_transformSystem;

private:
	void GetNextPattern(Boss& boss, BossAttackPattern& current, BossAttackPattern& previous);

	float GetNextDuration(BossAttackPattern current);

	float GetNextCoolDown(BossAttackPattern current);

	Vector3 GetNextPosition(BossAttackPattern& current, Transform& transform);

	void AttackA(World& world);

	void AttackB(World& world, Transform& transform);

	void AttackC(World& world, Transform& transform);

	void Damage(Status& status, RenderCommand& renderCommand, Boss& boss);

public:
	void Start(ComponentManager& cm, World& world) override;

	void Update(ComponentManager& cm, World& world) override;
};