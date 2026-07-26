#pragma once

struct GuardState : IComponentData
{
	bool isGuarding = false;
	float shieldMaxHealth = 100.0f;
	float shieldHealth = 100.0f;
	float shieldBreakTimer = 0.0f;
	bool isInit = false;
	Slider* bar = nullptr;
	bool isBroken = false;
};