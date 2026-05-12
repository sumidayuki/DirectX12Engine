#pragma once

enum class UIButtonState
{
	Normal,		// 通常状態
	Hovered,	// ホバー状態 (マウスオーバー)
	Selected,	// 選択状態 (クリックされた状態)
	Pressed,	// 押下状態
	Disabled,	// 無効状態
};

struct Button : IComponentData
{
	std::function<void()> onClick; // クリックされたときのコールバック関数

	UIButtonState state = UIButtonState::Normal; // ボタンの状態

	ComPtr<Sprite> sprite = nullptr; // ボタンに適用するスプライト (背景画像)
	Color normalColor = Color(0.8f, 0.8f, 0.8f, 1.0f);		// 通常状態の色
	Color hoveredColor = Color(0.9f, 0.9f, 0.9f, 1.0f);		// ホバー状態の色
	Color selectedColor = Color(0.7f, 0.7f, 0.7f, 1.0f);	// 選択状態の色
	Color pressedColor = Color(0.6f, 0.6f, 0.6f, 1.0f);		// 押下状態の色
	Color disabledColor = Color(0.5f, 0.5f, 0.5f, 1.0f);	// 無効状態の色

	bool wasClicked = false; // 前フレームでクリックされたかどうか
};