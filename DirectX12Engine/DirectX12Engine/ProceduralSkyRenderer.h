#pragma once

class ProceduralSkyRenderer
{
private:
	static inline float m_time;		// 現在の時間（0.0fから1.0fの範囲で、1.0fは24時間を表す）
	static inline float m_dayLength;	// 1日の長さ（秒）
	static inline bool m_isMoveTime;	// 時間を進めるかどうか
	static inline bool m_enableClouds;
	static inline bool m_enableStars;
	static inline bool m_enableMoon;
	static inline bool m_enableSun;
	static inline bool m_enableSunGlow;
	static inline ComPtr<GraphicsBuffer> m_skyConstantsBuffer; // ProceduralSkyConstants用の定数バッファ

public:
	static void Initialize(
		float time = 0.0f, 
		float dayLength = 60.0f, 
		bool isMoveTime = true, 
		bool enableClouds = true, 
		bool enableStars = true, 
		bool enableMoon = true, 
		bool enableSun = true, 
		bool enableSunGlow = true
	);

	static void Draw(World& world);
	static void SetTime(float time) { m_time = time; }
	static void SetDayLength(float dayLength)	{ m_dayLength = dayLength; }
	static void SetIsMoveTime(bool isMoveTime)	{ m_isMoveTime = isMoveTime; }
	static void SetEnableClouds(bool enable)	{ m_enableClouds = enable; }
	static void SetEnableStars(bool enable)		{ m_enableStars = enable; }
	static void SetEnableMoon(bool enable)		{ m_enableMoon = enable; }
	static void SetEnableSun(bool enable)		{ m_enableSun = enable; }
	static void SetEnableSunGlow(bool enable)	{ m_enableSunGlow = enable; }
};