#pragma once

enum class AttackInputType
{
	Idle,
	Attack1,
	Attack2,
	Attack3,
	Jump,
	Rolling
};

struct ComboInput : IComponentData
{
	AttackInputType attackInputType;
	float timer;
};