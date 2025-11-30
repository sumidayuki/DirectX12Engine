#pragma once

enum class PlayerState
{
	Move,
	Attack,
};

class PlayerSystem : public System
{
private:
	PlayerState m_currentState;

	Transform* m_cameraTransform;
	Transform* m_bowTransform;

	Transform* m_coll;

	float m_attackRange;

	float m_stateTimer;

	float m_currentSpeed;
	static constexpr float WalkSpeed = 75.0f;
	static constexpr float RunSpeed = 200.0f;
	static constexpr float MiddleSpeed = 100.0f;

private:
	void Move(Transform& transform, Input& input, Animator& anim);

	void Attack(Transform& transform, World& world);

private:
	void Start(World& world) override;
	
	void Update(World& world) override;
};