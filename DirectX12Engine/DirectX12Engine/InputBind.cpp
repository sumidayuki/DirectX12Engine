#include "InputBind.h"

InputBind::InputBind()
{
	m_keyBindMap[InputKey::Attack1] = KeyCode::Mouse0;
	m_keyBindMap[InputKey::Rolling] = KeyCode::Space;
	m_keyBindMap[InputKey::Guard] = KeyCode::LeftShift;
}