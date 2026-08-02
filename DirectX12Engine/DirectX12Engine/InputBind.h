#pragma once
#include "InputDeviceType.h"

enum class InputKey
{
	None,
	Attack1,
	Attack2,
	Rolling,
	Guard,
};

inline std::map<InputKey, std::string> InputKeyToString = {
	{ InputKey::Attack1, "Attack1" },
	{ InputKey::Attack2, "Attack2" },
	{ InputKey::Rolling, "Rolling" },
	{ InputKey::Guard, "Guard" },
};

inline std::map<std::string, InputKey> StringToInputKey = {
	{ "Attack1", InputKey::Attack1 },
	{ "Attack2", InputKey::Attack2 },
	{ "Rolling", InputKey::Rolling },
	{ "Guard", InputKey::Guard },
};

class InputBind
{
private:
	std::unordered_map<InputKey, KeyCode> m_keyBindMap;
	std::unordered_map<InputKey, GamepadButton> m_buttonBindMap; 
	std::unordered_map<InputKey, std::unordered_map<InputDeviceType, Sprite*>> m_spriteBindMap;

public:
	InputBind();

	void BindKey(InputKey inputKey, KeyCode keyCode) { m_keyBindMap[inputKey] = keyCode; }
	void BindButton(InputKey inputKey, GamepadButton button) { m_buttonBindMap[inputKey] = button; }
	void BindSprite(InputKey inputKey, InputDeviceType deviceType, Texture2D* texture);

	void UpdateSpriteBindMap();

	const std::unordered_map<InputKey, KeyCode>& GetKeyBindMap() const { return m_keyBindMap; }
	const std::unordered_map<InputKey, GamepadButton>& GetButtonMap() const { return m_buttonBindMap; }
	const std::unordered_map<InputKey, std::unordered_map<InputDeviceType, Sprite*>>& GetSpriteBindMap() const { return m_spriteBindMap; }
};