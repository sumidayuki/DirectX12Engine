#pragma once
#include "PlayerCamera.h"


class PlayerSystem : public System
{
private:
	GameManagerSystem* m_gameManager;

	Transform* m_cameraTransform;
	PlayerCamera* m_playerCamera;

	Transform* m_bowTransform;

	float m_currentSpeed;

	static constexpr float WalkSpeed = 100.0f;
	static constexpr float RunSpeed = 250.0f;

private:
	bool ProcessTurn(World& world, Transform& transform, Animator& animator, LocomotionData& loco);
	void Move(World& world, Transform& transform, Input& input, Animator& anim, LocomotionData& loco);
	void DrawArrow(Transform& transform, float speed, float damage, Animator& anim, World& world);
	void LegAttack(Transform& transform, MoveState& state, Animator& anim, Attackable& attackable, World& world);

private:
	void Start(World& world) override;

	void Update(World& world) override;
};