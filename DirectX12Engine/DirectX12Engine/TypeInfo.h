#pragma once
#include <cstdint>
#include <cstddef>
#include <type_traits>
#include <utility>

/// <summary>
/// Archetypeで型を識別する際の型情報を表す構造体です。
/// RTTI(typeid)を使わずにconstexpr比較ができます。
/// constexprを使用することで型情報や比較や生成がコンパイル時に最適化されます。
/// RTTIを使わないので軽量かつ移植性が高くなっています。
/// </summary>
struct TypeInfo
{
	const char* name;		// 型名
	uint64_t	hash;		// 型ハッシュ
	size_t		size;		// sizeof(T)
	size_t		alignment;	// alignof(T)
	void (*copy_construct)(void* dest, const void* src);
	void (*move_construct)(void* dest, void* src);
	void (*destroy)(void* target);

	constexpr TypeInfo(
		const char* name,
		uint64_t hash,
		size_t size,
		size_t alignment,
		void (*copy_construct)(void*, const void*),
		void (*move_construct)(void*, void*),
		void (*destroy)(void*)
	)
		: name(name)
		, hash(hash)
		, size(size)
		, alignment(alignment)
		, copy_construct(copy_construct)
		, move_construct(move_construct)
		, destroy(destroy)
	{
	}

	// 等価演算子
	constexpr bool operator==(const TypeInfo& rhs) const noexcept { return hash == rhs.hash && size == rhs.size; }
	// 不等価演算子
	constexpr bool operator!=(const TypeInfo& rhs) const noexcept { return !(*this == rhs); }
};

/// <summary>
/// FNV-1aハッシュ関数（constexpr対応）
/// 型名を64bit整数に変換します。
/// 文字列で型を比較すると重くなるので数値化(ハッシュ)
/// して整数比較を行うための関数です。
/// </summary>
constexpr uint64_t HashString(const char* str)
{
	return 	FNV1a_Hash<uint64_t>(str);
}

/// <summary>
/// コンパイル時に型名を取得するユーティリティ
/// RTTIを使用せずに関数名を取得するためのマクロを使用し
/// 多様なコンパイル化に対応しています。
/// </summary>
template <typename T>
constexpr const char* TypeName()
{
#if defined(__clang__) || defined(__GNUC__)
	// Clang / GCC でコンパイルする場合
	return __PRETTY_FUNCTION__;				// 例："const char* TypeName<int>()"
#elif defined(_MSC_VER)
	// Visual Studioでコンパイルする場合
	return __FUNCSIG__;						// 例："constexpr const char* TypeName() [T = int]"
#else
	// どれにも当てはまらない
	return "UnknownType";
#endif
}

/// <summary>
/// 型からTypeInfoを生成するテンプレート（RTTI非依存）
/// </summary>
template<typename T>
constexpr TypeInfo GetTypeInfo()
{
	constexpr const char* name = TypeName<T>();

	auto copy_construct = [](void* dest, const void* src)
	{
		if constexpr (!std::is_trivially_copyable_v<T>)
		{
			new (dest) T(*static_cast<const T*>(src));
		}
		else
		{
			memcpy(dest, src, sizeof(T));
		}
	};

	auto move_construct = [](void* dest, void* src)
	{
		if constexpr (!std::is_trivially_copyable_v<T>)
		{
			new (dest) T(std::move(*static_cast<T*>(src)));
			static_cast<T*>(src)->~T();
		}
		else
		{
			memcpy(dest, src, sizeof(T));
		}
	};

	auto destroy = [](void* target)
		{
			if constexpr (!std::is_trivially_destructible_v<T>)
			{
				static_cast<T*>(target)->~T();
			}
		};

	return TypeInfo(
		name,
		HashString(name),
		sizeof(T),
		alignof(T),
		copy_construct,
		move_construct,
		destroy
	);
}