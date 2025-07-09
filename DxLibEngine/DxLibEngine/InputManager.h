#pragma once
#include "ButtonControl.h"
#include "KeyCode.h"
#include "Gamepad.h"


//----------------------------------------------------------------------------------------------------------------------------------------------------
// ���̓f�o�C�X�����N���X
//----------------------------------------------------------------------------------------------------------------------------------------------------
class InputManager
{
public:
    // �ڑ��\�ȃQ�[���p�b�h�̍ő吔
    static inline const int MaxGamepadCount = XUSER_MAX_COUNT;
    friend class Application;

private:
    // �ʒm��E�B���h�E
    static inline HWND m_hInputWnd;

    // �Q�[���p�b�h�z��
    static inline Gamepad* m_gamepads[MaxGamepadCount];

private:
    // ���̓f�o�C�X�T�u�V�X�e�������������܂��B
    static void StaticConstructor(HWND hWnd);

    // ���̓f�o�C�X�T�u�V�X�e�����I�����܂��B
    static void StaticDestructor();

    // �ʒm��E�B���h�E���쐬���܂��B
    static HWND CreateInputWindow();

    // �E�B���h�E�v���V�[�W��
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    // WM_INPUT���̏���
    static void OnInput(HRAWINPUT hRawInput);

    // �}�E�X���͌��m���̏���
    static void OnInputMouse(const RAWMOUSE& mouse);

    // �L�[�{�[�h���͌��m���̏���
    static void OnInputKeyboard(const RAWKEYBOARD& keyboard);

    // ���̓f�o�C�X�T�u�V�X�e�����X�V���܂��B
    static void Update();

private:
    // �L�[��
    static constexpr int KeyCodeCount = 256;

    // �L�[�{�[�h/�}�E�X/�W���C�X�e�B�b�N�̃L�[��Ԕz��
    static inline bool m_keysLast[KeyCodeCount];
    static inline ButtonControl m_keys[KeyCodeCount];

public:
    // �L�[�{�[�h/�}�E�X/�W���C�X�e�B�b�N�̃L�[��Ԃ��擾���܂��B
    static const ButtonControl& GetKey(KeyCode key);


};


