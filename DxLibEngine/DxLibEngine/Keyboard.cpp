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
    else
    {
        OutputDebugStringA("GetKeyboardState failed!\n");
    }

    // デバッグ：キーの状態をログ出力
    if (m_keys[VK_LEFT].IsRepeated())
    {
        OutputDebugStringA("Left key is repeated!\n");
    }
}

const ButtonControl& Keyboard::GetKeyState(KeyCode key)
{
    return m_keys[(int)key];
}
