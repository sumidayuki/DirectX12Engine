#pragma once

enum class AttackInputType
{
	Idle,
	Attack1,
	Attack2,
	Attack3,
};

struct ComboInput : IComponentData
{
	AttackInputType attackInputType = AttackInputType::Idle;
	float timer = 0.0f;
};