#pragma once

enum class PlayerState
{
	Move,
	Attack,
};

class PlayerSystem : public System
{
private:
	GameManagerSystem* m_gameManager;

	PlayerState m_currentState;

	Transform* m_cameraTransform;
	Transform* m_bowTransform;

	Transform* m_coll;

	Collider* m_legAttackColl;

	Slider* m_hpBar;

	float m_attackRange;

	float m_stateTimer;

	float m_currentSpeed;
	static constexpr float WalkSpeed = 100.0f;
	static constexpr float RunSpeed = 250.0f;

private:
	bool ProcessTurn(World& world, Transform& transform, Animator& animator, LocomotionData& loco);
	void Move(World& world, Transform& transform, Input& input, Animator& anim, LocomotionData& loco);
	void DrawArrow(Transform& transform, float damage, Animator& anim, World& world);
	void LegAttack(Transform& transform, ComboState& state, Animator& anim, Attackable& attackable, World& world);

private:
	void Start(World& world) override;

	void Update(World& world) override;
};