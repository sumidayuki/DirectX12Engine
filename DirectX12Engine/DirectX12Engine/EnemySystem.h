#pragma once
#include "Enemy.h"

class EnemySystem : public System
{
private:
	Transform* m_hitBox;
	Collider* m_leftHandColl;
	Collider* m_rightHandColl;
	Collider* m_jumpAttackColl;

private:
	void Move(Enemy& enemy, AIAgent& aiAgent, Transform& transform, Animator& animator, World& world);
	void JampAttack(Entity& entity, Enemy& enemy, AIAgent& aiAgent, Transform& transform, ComboState& state, Animator& animator, World& world);
	bool ProcessCollision(World& world, Collider* coll, ComboState& state, Attackable& attackable);

private:
	void Start(World& world) override;
	void Update(World& world) override;
};