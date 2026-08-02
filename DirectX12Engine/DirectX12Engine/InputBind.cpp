#include "InputBind.h"

InputBind::InputBind()
{
	m_keyBindMap[InputKey::Attack1] = KeyCode::Mouse0;
	m_keyBindMap[InputKey::Rolling] = KeyCode::Space;
	m_keyBindMap[InputKey::Guard] = KeyCode::LeftShift;

	m_buttonBindMap[InputKey::Attack1] = GamepadButton::RightShoulder;
	m_buttonBindMap[InputKey::Rolling] = GamepadButton::South;
	m_buttonBindMap[InputKey::Guard] = GamepadButton::LeftShoulder;
	m_buttonBindMap[InputKey::Attack2] = GamepadButton::East;

	for (int i = 0; i < (int)InputDeviceType::Touch; i++)
	{
		m_spriteBindMap[InputKey::Attack1][(InputDeviceType)i] = nullptr;
		m_spriteBindMap[InputKey::Rolling][(InputDeviceType)i] = nullptr;
		m_spriteBindMap[InputKey::Guard][(InputDeviceType)i] = nullptr;
		m_spriteBindMap[InputKey::Attack2][(InputDeviceType)i] = nullptr;
	}
}

void InputBind::BindSprite(InputKey inputKey, InputDeviceType deviceType, Texture2D* texture)
{
	Rect spriteRect = { 0, 0, (float)texture->GetWidth(), (float)texture->GetHeight() };

	Sprite* newSprite = Sprite::Create(
		texture, 
		spriteRect, 
		Vector2(0.5f, 0.5f), 
		1.0f, 
		1.0f
	);
	m_spriteBindMap[inputKey][deviceType] = newSprite;
}
