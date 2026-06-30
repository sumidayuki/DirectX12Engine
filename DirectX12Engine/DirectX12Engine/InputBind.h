#pragma once

class InputBind
{
private:
	std::unordered_map<std::string, KeyCode> m_keyBindMap;
	std::unordered_map<std::string, GamepadButton> m_buttonBindMap; 

public:
	InputBind();

	const std::unordered_map<std::string, KeyCode>& GetKeyBindMap() const { return m_keyBindMap; }
	const std::unordered_map<std::string, GamepadButton>& GetButtonMap() const { return m_buttonBindMap; }
};