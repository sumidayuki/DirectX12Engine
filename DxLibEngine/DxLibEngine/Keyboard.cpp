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
            // i番目のキーが押されているか？
			const bool current = (states[i] & 0x80) != 0;

            // i番目のキーの状態を更新
            m_keys[i].Update(current);
        }
    }
}

const ButtonControl& Keyboard::GetKeyState(KeyCode key)
{
    return m_keys[(int)key];
}
