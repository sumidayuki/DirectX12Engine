#include "Keyboard.h"


void Keyboard::StaticConstructor()
{
    Update();
}


void Keyboard::StaticDestructor()
{

}


void Keyboard::Update()
{
    m_isPressedAnyKey = false;

    unsigned char states[256];
    if (GetKeyboardState(states))
    {
        m_isConnected = true;
        for (int i = 0; i < 256; i++)
        {
            // i番目のキーが押されているか？
            const bool current = (states[i] & 0x80) != 0;

            // i番目のキーの状態を更新
            m_keys[i].Update(current);

            if (current)
            {
                bool isIgnored = false;

                if (i <= 0x07) isIgnored = true;       // マウスボタン等を除外
                if (i == 0x14) isIgnored = true;       // Caps Lock
                if (i == 0x90) isIgnored = true;       // Num Lock
                if (i == 0x5B || i == 0x5C) isIgnored = true; // Left/Right Windows Key
                if (i == 240)  isIgnored = true;
				if (i == 244)  isIgnored = true;

                if (!isIgnored)
                {
                    m_isPressedAnyKey = true;
                }
            }
        }
    }
    else
    {
        m_isConnected = false;
    }
}


const ButtonControl& Keyboard::GetKeyState(KeyCode key)
{
    return m_keys[(int)key];
}
