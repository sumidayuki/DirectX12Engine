#pragma once

class Time
{
private:
	friend class Application;

private:
	// イベント時刻
	static inline std::chrono::time_point<std::chrono::steady_clock> m_startupTimePoint;
	static inline std::chrono::time_point<std::chrono::steady_clock> m_lastLevelLoadTime;

	// FPS
	static inline int		m_captureFramerate;
	static inline int		m_frameCount;
	static inline float		m_framesPerSecond;

	// タイムスケール
	static inline float		m_timeScale;

	// 固定時間ステップ
	static inline float		m_fixedTime;
	static inline float		m_fixedDeltaTime;
	static inline float		m_fixedUnscaledTime;
	static inline float		m_fixedUnscaledDeltaTime;
	static inline float		m_inFixedTimeStep;

	// 可変時間ステップ
	static inline float m_time;
	static inline float m_deltaTime;
	static inline float m_unscaledTime;
	static inline float m_unscaledDeltaTime;

	// 遅延上限
	static inline float m_maximumDeltaTime;

private:
	// 時間を初期化します。
	static void StaticConstructor();

public:
	static float GetDeltaTime();

	static float GetTime();
};