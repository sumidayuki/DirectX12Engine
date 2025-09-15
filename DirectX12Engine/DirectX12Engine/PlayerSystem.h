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

	float m_stateTimer;

	float m_currentSpeed;
	static constexpr float WalkSpeed = 75.0f;
	static constexpr float RunSpeed = 150.0f;
	static constexpr float MiddleSpeed = 100.0f;

private:
	void Move(Transform& transform, Input& input, Animator& anim);

	void Attack(Transform& transform, World& world);

private:
	void Start(ComponentManager& cm, World& world) override;
	
	void Update(ComponentManager& cm, World& world) override;
};