#pragma once
#include "InputBind.h"

struct MoveInput : IComponentData
{
	InputKey inputKey = InputKey::None;
	float timer = 0.0f;
};