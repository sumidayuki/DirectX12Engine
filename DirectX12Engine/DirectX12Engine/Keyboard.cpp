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
			const bool current = (states[i] & 0x80) != 0;

			m_keys[i].Update(current);

			bool isIgnored = false;

			if (i <= 0x07) isIgnored = true;
			if (i == VK_CAPITAL) isIgnored = true;
			if (i == VK_NUMLOCK) isIgnored = true;
			if (i == VK_LWIN || i == VK_RWIN) isIgnored = true;

			if (!isIgnored && m_keys[i].WasPressedThisFrame())
			{
				m_isPressedAnyKey = true;
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
