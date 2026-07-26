#pragma once
#include "Slider.h"

struct HP : IComponentData
{
	float maxHP;
	float currentHP;
	Slider* hpBar;
	bool isDeath = false;
	bool isInit = false;
	bool isSuperArmor = false;			// ダメージを受けた際にリアクションがあるか
	bool isInvincible = false;			// ダメージを受けるがHPを減らすか
	bool isSuperInvincible = false;		// ダメージをそもそも受けないか
};