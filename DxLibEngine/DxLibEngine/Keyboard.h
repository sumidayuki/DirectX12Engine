#pragma once

#include<DxLib.h>
#include "ButtonControl.h"
#include "KeyCode.h"

class Keyboard
{
private:
	// �L�[�{�[�h�̃L�[�̏�Ԃ��i�[����z��
	static inline ButtonControl m_keys[256];

public:
	// �L�[�{�[�h�̓��͏������������܂��B
	static void Initialize();

	// �L�[�{�[�h�̓��͏����X�V���܂��B�i���t���[���Ăяo���j
	static void Update();

	// �L�[�̖K����擾���܂��B
	static const ButtonControl& GetKeyState(KeyCode key);
};