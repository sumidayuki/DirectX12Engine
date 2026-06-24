#include "InputBind.h"

InputBind::InputBind()
{
	m_keyBindMap["Attack1"] = KeyCode::Mouse0;
	m_keyBindMap["Rolling"] = KeyCode::Space;
	m_keyBindMap["Guard"] = KeyCode::LeftShift;
}