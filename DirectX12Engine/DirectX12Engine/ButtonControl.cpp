#include "ButtonControl.h"

ButtonControl::ButtonControl()
	: m_current(false)
	, m_previous(false)
	, m_firstInterval(1.0f)
	, m_repeatInterval(0.5f)
	, m_elapsedTime(0.0f)
	, m_progress(Progress::Released)
{
}

void ButtonControl::Update(bool current)
{
	m_previous = m_current;
	m_current = current;

	if (m_current)
	{
		switch (m_progress)
		{
			case Progress::Released:
			{
				m_progress =	Progress::FirstPressed;
			}
			break;

			case Progress::FirstPressed:
			{
				m_progress =
					Progress::FirstInterval;
				m_elapsedTime = 0.0f;
			}
			[[fallthrough]];

			case Progress::FirstInterval:
			{
				m_elapsedTime += DeltaTime;
				if (m_elapsedTime >=
					m_firstInterval)
				{
					m_progress =
						Progress::RepeatPressed;
					m_elapsedTime =
						m_firstInterval;
				}
			}
			break;

			case Progress::RepeatPressed:
			{
				m_progress =
					Progress::RepeatInterval;
				m_elapsedTime = 0.0f;
			}
			[[fallthrough]];

			case Progress::RepeatInterval:
			{
				m_elapsedTime += DeltaTime;
				if (m_elapsedTime >=
					m_repeatInterval)
				{
					m_progress =
						Progress::RepeatPressed;
					m_elapsedTime -=
						m_repeatInterval;
				}
			}
			break;
		}
	}
	else
	{
		m_progress = Progress::Released;
		m_elapsedTime = 0.0f;
	}
}

bool ButtonControl::IsRepeated() const
{
	return (m_progress ==
	  Progress::FirstPressed ||
			m_progress ==
			  Progress::RepeatPressed);
}
