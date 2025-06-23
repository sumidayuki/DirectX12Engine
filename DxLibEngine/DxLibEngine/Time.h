#pragma once

class Time
{
private:
	friend class Application;

private:
	// �C�x���g����
	static inline std::chrono::time_point<std::chrono::steady_clock> m_startupTimePoint;
	static inline std::chrono::time_point<std::chrono::steady_clock> m_lastLevelLoadTime;

	// FPS
	static inline int		m_captureFramerate;
	static inline int		m_frameCount;
	static inline float		m_framesPerSecond;

	// �^�C���X�P�[��
	static inline float		m_timeScale;

	// �Œ莞�ԃX�e�b�v
	static inline float		m_fixedTime;
	static inline float		m_fixedDeltaTime;
	static inline float		m_fixedUnscaledTime;
	static inline float		m_fixedUnscaledDeltaTime;
	static inline float		m_inFixedTimeStep;

	// �ώ��ԃX�e�b�v
	static inline float m_time;
	static inline float m_deltaTime;
	static inline float m_unscaledTime;
	static inline float m_unscaledDeltaTime;

	// �x�����
	static inline float m_maximumDeltaTime;

private:
	// ���Ԃ����������܂��B
	static void StaticConstructor();

public:
	static float GetDeltaTime();

	static float GetTime();
};