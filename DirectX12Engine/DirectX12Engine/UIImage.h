#pragma once
#include "UIObject.h"

/// <summary>
/// スクリーン座標でUI画像を描画するためのコンポーネントです。
/// UnityのCanvasのように、ピクセル座標で位置とサイズを指定できます。
/// 座標系は左上(0,0)、右下(screenWidth, screenHeight)です。
/// </summary>
struct UIImage : UIObject
{
	Vector2					position = Vector2::zero;
	Vector2					size = Vector2(100.0f, 100.0f);
	Vector2					pivot = Vector2(0.0f, 0.0f);
	Color					color = Color::white;

	ComPtr<Sprite>			sprite = nullptr;
	ComPtr<GraphicsBuffer>	constantBuffer = nullptr;
	ComPtr<DescriptorHeap>	descriptorHeap = nullptr;
};
