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
	Vector3 position = { 0.0f, 0.0f, 0.0f };		// Point, Spotで使用
	float  range = 10.0f;						// Point, Spotで使用（光の届く最大距離）
	Vector3 direction = { 0.0f, -1.0f, 0.0f };	// Directional, Spotで使用
	float  spotAngle = 30.0f;					// Spotで使用（円錐の角度）

	LightType type = LightType::Directional;
	int enabled = 1;                           // int (4 bytes) - activeLightsに入れるので通常は1
	float padding[2] = { 0.0f, 0.0f };          // float2 (8 bytes)
};