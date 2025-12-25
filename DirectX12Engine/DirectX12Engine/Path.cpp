#include "Path.h"
#include <windows.h>
#include <cassert>

#if defined(_WIN32)
// Windows common
const wchar_t Path::VolumeSeparatorChar = L':';
const wchar_t Path::PathSeparator = L';';
const wchar_t Path::DirectorySeparatorChar = L'\\';
const wchar_t Path::AltDirectorySeparatorChar = L'/';
#elif defined(__APPLE__)
// MacOS common
const wchar_t Path::VolumeSeparatorChar = L':';
const wchar_t Path::PathSeparator = L';';
const wchar_t Path::DirectorySeparatorChar = L':';
const wchar_t Path::AltDirectorySeparatorChar = L'/';
#elif defined(__linux__)
// Linux common
const wchar_t Path::VolumeSeparatorChar = L'/';
const wchar_t Path::PathSeparator = L';';
const wchar_t Path::DirectorySeparatorChar = L'/';
const wchar_t Path::AltDirectorySeparatorChar = L'\\';
#else
// other OS
const wchar_t Path::VolumeSeparatorChar = L':';
const wchar_t Path::PathSeparator = L';';
const wchar_t Path::DirectorySeparatorChar = L'\\';
const wchar_t Path::AltDirectorySeparatorChar = L'/';
#endif


// 無効な文字
static const wchar_t InvalidPathCharsArray[] =
{
    0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F,
    0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E, 0x001F,
    L'"', L'<', L'>', L'|',
};

static const std::vector<wchar_t> InvalidPathCharsVector
{
    0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F,
    0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E, 0x001F,
    L'"', L'<', L'>', L'|',
};

static const std::wstring DirectorySeparatorString
{
    Path::DirectorySeparatorChar,
    Path::AltDirectorySeparatorChar,
    L'\0',
};


const std::vector<wchar_t>& Path::GetInvalidPathChars()
{
    return InvalidPathCharsVector;
}

const std::vector<wchar_t>& Path::GetInvalidFileNameChars()
{
    return InvalidPathCharsVector;
}


bool Path::IsPathRooted(const std::wstring& path)
{
    return (path.length() >= 2) && (path[1] == L':');
}


std::wstring Path::GetPathRoot(const std::wstring& path)
{
    // 最初のディレクトリセパレータ―
    const size_t indexOfFirstSeparator = path.find_first_of(DirectorySeparatorString);
    if (indexOfFirstSeparator != std::wstring::npos)
    {
        return path.substr(0, indexOfFirstSeparator + 1);
    }
    else
    {
        return std::wstring();
    }
}


std::wstring Path::GetFullPath(const std::wstring& path)
{
    wchar_t fullPath[MAX_PATH];
    if (!GetFullPathNameW(path.c_str(), _countof(fullPath), fullPath, nullptr))
    {
        assert(0);
    }

    return fullPath;
}


std::wstring Path::GetDirectoryName(const std::wstring& path)
{
    // 最後のディレクトリセパレータ―
    const size_t indexOfLastSeparator = path.find_last_of(DirectorySeparatorString);
    if (indexOfLastSeparator != std::wstring::npos)
    {
        return path.substr(0, indexOfLastSeparator);
    }
    else
    {
        return std::wstring();
    }
}


std::wstring Path::GetFileName(const std::wstring& path)
{
    // 最後のディレクトリセパレータ―
    const size_t indexOfLastSeparator = path.find_last_of(DirectorySeparatorString);
    if (indexOfLastSeparator != std::wstring::npos)
    {
        return path.substr(indexOfLastSeparator + 1);
    }
    else
    {
        return std::wstring();
    }
}


std::wstring Path::GetFileNameWithoutExtension(const std::wstring& path)
{
    size_t offset = 0;

    // 最後のディレクトリセパレータ―
    const size_t indexOfLastSeparator = path.find_last_of(DirectorySeparatorString);
    if (indexOfLastSeparator != std::wstring::npos)
    {
        offset = indexOfLastSeparator;
    }

    size_t count = std::wstring::npos;

    // 最後のピリオド
    const size_t indexOfLastPeriod = path.find(L'.', offset);
    if (indexOfLastPeriod != std::wstring::npos)
    {
        count = indexOfLastPeriod - indexOfLastSeparator - 1;
    }

    return path.substr(indexOfLastSeparator + 1, count);
}


bool Path::HasExtension(const std::wstring& path)
{
    size_t offset = 0;

    // 最後のディレクトリセパレータ―
    const size_t indexOfLastSeparator = path.find_last_of(DirectorySeparatorString);
    if (indexOfLastSeparator != std::wstring::npos)
    {
        offset = indexOfLastSeparator;
    }

    // 最後のピリオド
    const size_t indexOfLastPeriod = path.find(L'.', offset);
    if (indexOfLastPeriod != std::wstring::npos)
    {
        return (indexOfLastPeriod + 1) < path.length();
    }
    else
    {
        return false;
    }
}


std::wstring Path::GetExtension(const std::wstring& path)
{
    size_t offset = 0;

    // 最後のディレクトリセパレータ―
    const size_t indexOfLastSeparator = path.find_last_of(DirectorySeparatorString);
    if (indexOfLastSeparator != std::wstring::npos)
    {
        offset = indexOfLastSeparator;
    }

    // 最後のピリオド
    const size_t indexOfLastPeriod = path.find(L'.', offset);
    if (indexOfLastPeriod != std::wstring::npos)
    {
        return path.substr(indexOfLastPeriod);
    }
    else
    {
        return std::wstring();
    }
}


std::wstring Path::GetTempPathW()
{
    return std::wstring();
}


std::wstring Path::GetTempFileNameW()
{
    return std::wstring();
}


