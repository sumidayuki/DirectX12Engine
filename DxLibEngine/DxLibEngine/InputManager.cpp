#include "InputManager.h"

void InputManager::Initialize(HWND hWnd)
{
	// キーボードを初期化
	Keyboard::Initialize();
}

void InputManager::Update()
{
	// キーボードの入力状態を更新
	Keyboard::Update();
}
