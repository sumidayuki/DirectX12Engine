#pragma once
#include "InputBind.h"

struct MoveInput : IComponentData
{
	// 押されたMove入力
	InputKey inputKey = InputKey::None;

	// 離されたMove入力
	InputKey releasedKey = InputKey::None;

	// inputKeyの入力バッファ時間
	float timer = 0.0f;
};