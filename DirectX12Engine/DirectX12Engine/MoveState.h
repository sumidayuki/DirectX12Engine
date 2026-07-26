#pragma once

struct MoveState : IComponentData
{
	std::string name = "";
	uint32_t currentMoveId = 0;
	int comboIndex = 0;
	bool canHit = false;
	bool hitConfirm = false;
	bool isAnimed = false;
	float timer = 0.0f;
};