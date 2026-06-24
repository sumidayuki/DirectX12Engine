#pragma once

template<class T>
constexpr T FNV_Prime;

template<class T>
constexpr T FNV_Offset_Basis;

template<>
constexpr std::uint32_t FNV_Prime<std::uint32_t> = 16777619u;

template<>
constexpr std::uint64_t FNV_Prime<std::uint64_t> = 1099511628211ull;

template<>
constexpr std::uint32_t FNV_Offset_Basis<std::uint32_t> = 2166136261u;

template<>
constexpr std::uint64_t FNV_Offset_Basis<std::uint64_t> = 14695981039346656037ull;

template<class T>
constexpr T FNV1a_Hash(const void* data, size_t size) noexcept
{
    const unsigned char* bytes = static_cast<const unsigned char*>(data);
    T hash = FNV_Offset_Basis<T>;

    for (size_t i = 0; i < size; ++i)
    {
        hash ^= bytes[i];
        hash *= FNV_Prime<T>;
    }

    return hash;
}

template<class T>
constexpr T FNV1a_Hash(std::string_view str) noexcept
{
    T hash = FNV_Offset_Basis<T>;
    for (auto c : str)
    {
        hash ^= c;
        hash *= FNV_Prime<T>;
    }

    return hash;
}

constexpr std::uint32_t operator ""_h(const char* str, std::size_t len) noexcept
{
    return FNV1a_Hash<std::uint32_t>(std::string_view(str, len));
}

constexpr std::uint64_t operator ""_h64(const char* str, std::size_t len) noexcept
{
    return FNV1a_Hash<std::uint64_t>(std::string_view(str, len));
}