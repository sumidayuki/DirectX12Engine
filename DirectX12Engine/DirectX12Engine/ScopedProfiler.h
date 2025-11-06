#pragma once
#include <chrono>
#include <string>
#include <Windows.h>

class ScopedProfiler
{
public:
    ScopedProfiler(const std::string& name)
        : m_name(name)
    {
        m_start = std::chrono::high_resolution_clock::now();
    }

    ~ScopedProfiler()
    {
        auto end = std::chrono::high_resolution_clock::now();
        double ms = std::chrono::duration<double, std::milli>(end - m_start).count();

        char buffer[128];
        sprintf_s(buffer, "%s took: %.3f ms\n", m_name.c_str(), ms);
        OutputDebugStringA(buffer); // ← コンソールを使わず出力ウィンドウに出す！
    }

private:
    std::string m_name;
    std::chrono::high_resolution_clock::time_point m_start;
};

#define PROFILE_SCOPE(name) ScopedProfiler __profiler__(name);
