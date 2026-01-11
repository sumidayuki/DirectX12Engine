#pragma once

struct ComboState : IComponentData
{
	std::string name = "";
	int currentMoveID = 0;
	int comboIndex = 0;
	float timer = 0.0f;
};