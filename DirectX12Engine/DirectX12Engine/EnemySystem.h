#pragma once
#include "Enemy.h"

class EnemySystem : public System
{
private:
	Transform* m_hitBox;
	Collider* m_leftHandColl;
	Collider* m_rightHandColl;

private:
	float GetTargetDistance(const Vector3 enemyPos, const Vector3 targetpos) const;

	void Idle(Entity& entity, Enemy& enemy, Transform& transform, Animator& animator);
	void Chase(Entity& entity, Enemy& enemy, AIAgent& aiAgent, Transform& transform, Animator& animator, World& world);
	void Attack(Entity& entity, Enemy& enemy, Transform& transform, Animator& animator);

private:
	void Start(World& world) override;
	void Update(World& world) override;
};