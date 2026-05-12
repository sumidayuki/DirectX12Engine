#pragma once

struct Slider : IComponentData
{
	float value = 0.5f; // 0.0f ~ 1.0f
	float minValue = 0.0f;
	float maxValue = 1.0f;

	bool isLeftToRight = true; // スライダーの向き (左から右に増加するかどうか)

	ComPtr<Sprite> backgroundSprite = nullptr;
	Color backgroundColor = Color(0.5f, 0.5f, 0.5f, 1.0f);
	ComPtr<Sprite> fillSprite = nullptr;
	Color fillColor = Color(0.2f, 0.7f, 1.0f, 1.0f);

	bool isDragging = false; // ドラッグ中かどうか
};