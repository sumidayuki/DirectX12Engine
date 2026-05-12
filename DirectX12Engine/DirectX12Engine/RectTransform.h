#pragma once

struct RectTransform : IComponentData
{
	Vector2 anchoredPosition = Vector2::zero; // アンカーからのオフセット位置 (ピクセル単位)

	Vector2 sizeDelta = Vector2(100.0f, 100.0f); // サイズのオフセット (ピクセル単位)

	Vector2 anchorMin = Vector2(0.5f, 0.5f); // アンカーの最小値 (0.0f ~ 1.0f)
	Vector2 anchorMax = Vector2(0.5f, 0.5f); // アンカーの最大値 (0.0f ~ 1.0f)

	Vector2 pivot = Vector2(0.5f, 0.5f); // ピボット (0.0f ~ 1.0f)

	Vector2 rectMin = Vector2::zero; // 計算された矩形の最小値 (ピクセル単位)
	Vector2 rectMax = Vector2::zero; // 計算された矩形の最大値 (ピクセル単位)
};