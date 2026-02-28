#include "InputControls.h"

void Vector2Control::Update()
{
}

void StickControl::Update(const Vector2& value)
{
	m_x.SetValue(value.x);
	m_y.SetValue(value.y);

	// è‡’l‚ðÝ’èi0.5ˆÈã‚Å“ü—Í‚ ‚è‚Æ”»’èj
	const float threshold = 0.5f;

	m_up.Update(value.y > threshold);
	m_down.Update(value.y < -threshold);
	m_right.Update(value.x > threshold);
	m_left.Update(value.x < -threshold);
}
