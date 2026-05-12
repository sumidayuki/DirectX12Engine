#pragma once
#include "Enemy.h"

class EnemySystem : public System
{
private:
	bool m_isInvincivle;
	Transform* m_hitBox;
	Collider* m_leftHandColl;
	Collider* m_rightHandColl;
	Collider* m_jumpAttackColl;
	Slider* m_hpBar;

private:
	bool ProcessTurn(World& world, Transform& transform, Animator& animator, LocomotionData& loco);
	void Move(World& world, Enemy& enemy, AIAgent& aiAgent, Transform& transform, Animator& animator, LocomotionData& loco);
	void JumpAttack(Entity& entity, Enemy& enemy, AIAgent& aiAgent, Transform& transform, ComboState& state, Animator& animator, World& world);
	bool ProcessCollision(World& world, Collider* coll, ComboState& state, Attackable& attackable);

private:
	void Start(World& world) override;
	void Update(World& world) override;
};