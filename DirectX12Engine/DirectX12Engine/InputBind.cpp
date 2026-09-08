#include "InputBind.h"
#include "JSONImporter.h"
#include "InputCodeMap.h"

InputBind::InputBind()
{
	JSONImporter importer;
	Json json = importer.Import(L"Assets/Json/Input/InputBind.json");

	const Json& attack1 = json.value("Attack1", Json::object());
	const Json& attack2 = json.value("Attack2", Json::object());
	const Json& rolling = json.value("Rolling", Json::object());
	const Json& guard = json.value("Guard", Json::object());

	m_keyBindMap[InputKey::Attack1] = StringToKeyCode.at(attack1.value("Key", "mouse_left"));
	m_keyBindMap[InputKey::Attack2] = StringToKeyCode.at(attack2.value("Key", "mouse_right"));
	m_keyBindMap[InputKey::Rolling] = StringToKeyCode.at(rolling.value("Key", "space"));
	m_keyBindMap[InputKey::Guard] = StringToKeyCode.at(guard.value("Key", "left_shift"));

	m_buttonBindMap[InputKey::Attack1] = StringToGamepadButton.at(attack1.value("Gamepad", "gamepad_right_shoulder"));
	m_buttonBindMap[InputKey::Attack2] = StringToGamepadButton.at(attack2.value("Gamepad", "gamepad_east"));
	m_buttonBindMap[InputKey::Rolling] = StringToGamepadButton.at(rolling.value("Gamepad", "gamepad_south"));
	m_buttonBindMap[InputKey::Guard] = StringToGamepadButton.at(guard.value("Gamepad", "gamepad_left_shoulder"));

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
	if (!texture)
	{
		texture = AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/white.png");
	}

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
