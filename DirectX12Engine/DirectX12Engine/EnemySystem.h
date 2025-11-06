#pragma once
#include "Enemy.h"

class EnemySystem : public System
{
private:
	float GetTargetDistance(const Vector3 enemyPos, const Vector3 targetpos) const;

	void Idle(Entity& entity, Enemy& enemy, Transform& transform, Animator& animator);
	void Chase(Entity& entity, Enemy& enemy, AIAgent& aiAgent, Transform& transform, Animator& animator, World& world);
	void Attack(Entity& entity, Enemy& enemy, Transform& transform, Animator& animator);

private:
	void Start(ComponentManager& cm, World& world) override;
	void Update(ComponentManager& cm, World& world) override;
};