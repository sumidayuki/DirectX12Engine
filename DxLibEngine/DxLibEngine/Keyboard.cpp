#include "Keyboard.h"

void Keyboard::Initialize()
{
    for (int i = 0; i < 256; i++)
    {
        m_keys[i].Update(false);
    }
}

void Keyboard::Update()
{
    unsigned char states[256];
    if (GetKeyboardState(states))
    {
        for(int i = 0; i < 256; i++)
        {
            // i�Ԗڂ̃L�[��������Ă��邩�H
			const bool current = (states[i] & 0x80) != 0;

            // i�Ԗڂ̃L�[�̏�Ԃ��X�V
            m_keys[i].Update(current);
        }
    }
    else
    {
        OutputDebugStringA("GetKeyboardState failed!\n");
    }

    // �f�o�b�O�F�L�[�̏�Ԃ����O�o��
    if (m_keys[VK_LEFT].IsRepeated())
    {
        OutputDebugStringA("Left key is repeated!\n");
    }
}

const ButtonControl& Keyboard::GetKeyState(KeyCode key)
{
    return m_keys[(int)key];
}
