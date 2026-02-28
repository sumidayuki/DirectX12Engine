#pragma once

struct GuardState : IComponentData
{
	bool isGuarding = false;
	float shieldMaxHealth = 100.0f;
	float shieldHealth = 100.0f;
	bool isBroken = false;
};