#pragma once
#include "PlayerCamera.h"

enum class PlayerState
{
	Move,
	Attack,
	Step,
};

class PlayerSystem : public System
{
private:
	GameManagerSystem* m_gameManager;

	PlayerState m_currentState;

	Transform* m_cameraTransform;
	PlayerCamera* m_playerCamera;

	Transform* m_bowTransform;

	Slider* m_hpBar;

	float m_attackRange;

	float m_stateTimer;

	float m_currentSpeed;
	static constexpr float WalkSpeed = 100.0f;
	static constexpr float RunSpeed = 250.0f;

private:
	bool ProcessTurn(World& world, Transform& transform, Animator& animator, LocomotionData& loco);
	void Move(World& world, Transform& transform, Input& input, Animator& anim, LocomotionData& loco, RollingState& rolling, Stamina& stamina);
	void DrawArrow(Transform& transform, float speed, float damage, Animator& anim, World& world);
	void LegAttack(Transform& transform, MoveState& state, Animator& anim, Attackable& attackable, World& world);

private:
	void Start(World& world) override;

	void Update(World& world) override;
};