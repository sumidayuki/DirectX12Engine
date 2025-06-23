#pragma once

class Rect
{
public:
	float x;		// 矩形の左上隅X座標
	float y;		// 矩形の左上隅Y座標
	float width;	// 矩形の横幅
	float height;	// 矩形の高さ

public:
	// デフォルトコンストラクタ
	Rect() = default;

	// 引数付きコンストラクタ
	Rect(float x, float y, float width, float height);
};