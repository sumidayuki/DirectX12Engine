#pragma once

class Screen
{
private:
	static inline int m_width;
	static inline int m_height;
	static inline bool m_fullScreen;

public:
	static void SetResolution(int width, int height, bool fullScreen);

	static int GetWidth() { return m_width; }

	static int GetHeight() { return m_height; }

	static bool IsFullScreen() { return m_fullScreen; }

	static std::vector<Resolution> GetResolution();
};