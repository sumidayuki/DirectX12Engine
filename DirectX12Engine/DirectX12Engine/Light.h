#pragma once

enum class LightType
{
	Directional = 0, 	// 無限遠から均一に光を当てる
	Point = 1,			// 点から球状に光を当てる
	Spot = 2,			// 円錐状に光を当てる
};

struct Light : IComponentData
{
	Color color = { 1.0f, 1.0f, 1.0f, 1.0f };	// ライトの色と強度
	float  range = 10.0f;						// Point, Spotで使用（光の届く最大距離）
	float  spotAngle = 30.0f;					// Spotで使用（円錐の角度）

	LightType type = LightType::Directional;
};