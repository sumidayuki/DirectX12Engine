#pragma once
#include "Enemy.h"

class EnemySystem : public System
{
private:
	bool ProcessTurn(World& world, Transform& transform, Animator& animator, LocomotionData& loco);
	void Move(World& world, Enemy& enemy, AIAgent& aiAgent, Transform& transform, Animator& animator, LocomotionData& loco);
	void Miai(World& world, Enemy& enemy, AIAgent& aiAgent, Transform& transform, CharacterStatus& status);	
	void Approach(World& world, Enemy& enemy, AIAgent& aiAgent, Transform& transform, CharacterStatus& status);
	void JumpAttack(Entity& entity, Enemy& enemy, AIAgent& aiAgent, Transform& transform, MoveState& state, Animator& animator, World& world);

private:
	void Start(World& world) override;
	void Update(World& world) override;
};