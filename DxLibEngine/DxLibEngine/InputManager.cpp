#include "InputManager.h"

void InputManager::Initialize(HWND hWnd)
{
	// �L�[�{�[�h��������
	Keyboard::Initialize();
}

void InputManager::Update()
{
	// �L�[�{�[�h�̓��͏�Ԃ��X�V
	Keyboard::Update();
}
