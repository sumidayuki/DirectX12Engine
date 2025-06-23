#pragma once
#include<DxLib.h>
#include "Keyboard.h"
#include "KeyCode.h"

class InputManager
{
public:
	// 入力デバイスサブシステムを初期化します。
	static void Initialize(HWND hWnd);

	// 入力デバイスサブシステムを更新します。
	static void Update();
};
