#pragma once
#include "Slider.h"

struct HP : IComponentData
{
	float maxHP;
	float currentHP;
	Slider* hpBar;
	bool isDeath = false;
	bool isInit = false;
	bool isInvincible = false;			// ダメージを受けるがHPを減らさない
	bool isSuperInvincible = false;		// ダメージを受けない
};