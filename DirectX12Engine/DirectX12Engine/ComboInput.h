#pragma once
#include "InputBind.h"

enum class AttackInputType
{
	Idle,
	Attack1,
	Attack2,
	Attack3,
};

struct ComboInput : IComponentData
{
	InputKey inputKey = InputKey::None;
	float timer = 0.0f;
};