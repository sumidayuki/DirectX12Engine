#pragma once

// C++とHLSLで構造体を共有するための準備
#ifdef __cplusplus
using float3 = Vector3;
using float4 = Color;
#endif


enum class LightType
{
	Directional = 0, 	// 無限遠から均一に光を当てる
	Point = 1,			// 点から球状に光を当てる
	Spot = 2,			// 円錐状に光を当てる
};

struct Light
{
	float4 color = { 1.0f, 1.0f, 1.0f, 1.0f };	// ライトの色と強度
	float3 position = { 0.0f, 0.0f, 0.0f };		// Point, Spotで使用
	float  range = 10.0f;						// Point, Spotで使用（光の届く最大距離）
	float3 direction = { 0.0f, -1.0f, 0.0f };	// Directional, Spotで使用
	float  spotAngle = 30.0f;					// Spotで使用（円錐の角度）

	LightType type = LightType::Directional;
	int enabled = 1;							// 有効フラグ (boolはHLSLとの互換性で問題が起きやすいためint)

	// HLSLのStructuredBufferの要素は16バイトアライメントされるため、
	// C++側でもサイズを合わせる
	float padding[2];

};