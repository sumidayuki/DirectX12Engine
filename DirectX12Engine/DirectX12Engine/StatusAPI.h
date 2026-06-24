#pragma once

/// <summary>
/// CharacterStatus コンポーネントへの Get/Set アクセスを提供するユーティリティ。
/// コンポーネント自体は純粋なデータ構造のまま保ちます。
/// </summary>
namespace StatusAPI
{
	// ===== Float =====
	static inline void SetFloat(CharacterStatus& status, const std::string& key, float value)
	{
		status.floats[key] = value;
	}

	static inline float GetFloat(const CharacterStatus& status, const std::string& key, float defaultValue = 0.0f)
	{
		auto it = status.floats.find(key);
		return it != status.floats.end() ? it->second : defaultValue;
	}

	// ===== Int =====
	static inline void SetInt(CharacterStatus& status, const std::string& key, int value)
	{
		status.ints[key] = value;
	}

	static inline int GetInt(const CharacterStatus& status, const std::string& key, int defaultValue = 0)
	{
		auto it = status.ints.find(key);
		return it != status.ints.end() ? it->second : defaultValue;
	}

	// ===== Bool =====
	static inline void SetBool(CharacterStatus& status, const std::string& key, bool value)
	{
		status.bools[key] = value;
	}

	static inline bool GetBool(const CharacterStatus& status, const std::string& key, bool defaultValue = false)
	{
		auto it = status.bools.find(key);
		return it != status.bools.end() ? it->second : defaultValue;
	}

	// ===== Vector3 =====
	static inline void SetVector(CharacterStatus& status, const std::string& key, const Vector3& value)
	{
		status.vectors[key] = value;
	}

	static inline Vector3 GetVector(const CharacterStatus& status, const std::string& key, const Vector3& defaultValue = Vector3::zero)
	{
		auto it = status.vectors.find(key);
		return it != status.vectors.end() ? it->second : defaultValue;
	}

	// ===== String =====
	static inline void SetString(CharacterStatus& status, const std::string& key, const std::string& value)
	{
		status.strings[key] = value;
	}

	static inline std::string GetString(const CharacterStatus& status, const std::string& key, const std::string& defaultValue = "")
	{
		auto it = status.strings.find(key);
		return it != status.strings.end() ? it->second : defaultValue;
	}

	// ===== 存在チェック =====
	static inline bool HasFloat(const CharacterStatus& status, const std::string& key)
	{
		return status.floats.count(key) > 0;
	}

	static inline bool HasInt(const CharacterStatus& status, const std::string& key)
	{
		return status.ints.count(key) > 0;
	}

	static inline bool HasBool(const CharacterStatus& status, const std::string& key)
	{
		return status.bools.count(key) > 0;
	}

	static inline bool HasVector(const CharacterStatus& status, const std::string& key)
	{
		return status.vectors.count(key) > 0;
	}

	static inline bool HasString(const CharacterStatus& status, const std::string& key)
	{
		return status.strings.count(key) > 0;
	}
}
