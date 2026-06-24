#pragma once

struct Stamina : IComponentData
{
	const char* barName;		// スタミナバーの名前
	float value = 100;			// スタミナの現在値
	float maxValue = 100;		// スタミナの最大値
	float timer = 0.0f;			// スタミナが減少してからの経過時間
	float regenDelay = 5.0f;	// スタミナが減少してから回復が始まるまでの時間
	float regenRate = 10.0f;	// スタミナの回復速度（1秒あたりの回復量）
};