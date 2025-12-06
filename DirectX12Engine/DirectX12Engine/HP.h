#pragma once

struct HP : IComponentData
{
	float maxHP;
	float currentHP;
	bool isDeath = false;
	bool isInit = false;
};