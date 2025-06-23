#include "Precompiled.h"
#include "Time.h"

void Time::StaticConstructor()
{
    // �C�x���g����
    m_startupTimePoint = std::chrono::high_resolution_clock::now();
    m_lastLevelLoadTime = std::chrono::high_resolution_clock::now();

    // FPS
    m_captureFramerate = 60;
    m_frameCount = 0;
    m_framesPerSecond = 0.0f;

    // �^�C���X�P�[��
    m_timeScale = 1.0f;

    // �Œ莞�ԃX�e�b�v
    m_fixedTime = 0.0f;
    m_fixedDeltaTime = 1.0f / 60;
    m_fixedUnscaledTime = 0.0f;
    m_fixedUnscaledDeltaTime = 0.0f;
    m_inFixedTimeStep = false;

    // �ϓ����ԃX�e�b�v
    m_time = 0.0f;
    m_deltaTime = 0.0f;
    m_unscaledTime = 0.0f;
    m_unscaledDeltaTime = 0.0f;

    // �x�����
    m_maximumDeltaTime = 1.0f / 10;
}

float Time::GetDeltaTime()
{
    return m_inFixedTimeStep ? m_fixedDeltaTime : m_deltaTime;
}

float Time::GetTime()
{
    return m_inFixedTimeStep ? m_fixedTime : m_time;
}
