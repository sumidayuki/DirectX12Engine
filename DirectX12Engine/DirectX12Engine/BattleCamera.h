#pragma once

struct BattleCamera : IComponentData
{
	Entity character1;
	Entity character2;
	float aspect;

	bool isStarted = false;
};