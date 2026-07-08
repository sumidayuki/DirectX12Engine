#pragma once

struct ComboState : IComponentData
{
	std::string name = "";
	uint32_t currentMoveId = 0;
	int comboIndex = 0;
	bool isAttack = false;
	bool canHit = false;
	bool hitConfirm = false;
	bool isAnimed = false;
	float timer = 0.0f;
};