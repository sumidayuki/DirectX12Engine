#pragma once

struct ComboState : IComponentData
{
	std::string name = "";
	int currentMoveId = 0;
	int comboIndex = 0;
	bool canHit = false;
	bool hitConfirm = false;
	bool isAnimed = false;
	float timer = 0.0f;
};