#pragma once
#include<DxLib.h>
#include "Keyboard.h"
#include "KeyCode.h"

class InputManager
{
public:
	// ���̓f�o�C�X�T�u�V�X�e�������������܂��B
	static void Initialize(HWND hWnd);

	// ���̓f�o�C�X�T�u�V�X�e�����X�V���܂��B
	static void Update();
};
