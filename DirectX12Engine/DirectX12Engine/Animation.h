#pragma once

// アニメーションの各キーフレーム
template<typename T>
struct Keyframe
{
	T value;
	float timeStamp;
};

// 1つのボーンのアニメーションデータ
struct BoneAnimation
{
	std::vector<Keyframe<Vector3>>		positions;
	std::vector<Keyframe<Quaternion>>	rotations;
	std::vector<Keyframe<Vector3>>		scales;
};

class Animation : public Reference
{
private: 
	std::string m_name;													// アニメーション名
	float m_duration;													// アニメーションの長さ
	float m_ticksPerSecond;												// 1秒あたりのtick数
	std::unordered_map<std::string, BoneAnimation> m_boneAnimations;	// ボーン名をキーとしたアニメーションデータ

public:
	/// <summary>
	/// アニメーション名を取得します。
	/// </summary>
	/// <returns></returns>
	const std::string& GetName() const { return m_name; }

	/// <summary>
	/// アニメーションの長さ（ticks単位）を取得します。
	/// </summary>
	/// <returns></returns>
	float GetDuration() const { return m_duration; }

	/// <summary>
	/// 1秒あたりのtick数を取得します。
	/// </summary>
	/// <returns></returns>
	float GetTicksPerSecond() const { return m_ticksPerSecond; }

	/// <summary>
	/// アニメーションの総再生時間を秒で取得します。
	/// </summary>
	/// <returns></returns>
	float GetTotalTime() const { return m_duration / m_ticksPerSecond; }

	/// <summary>
	/// アニメーション名を設定します。
	/// </summary>
	/// <param name="name"></param>
	void SetName(const std::string& name) { m_name = name; }

	/// <summary>
	/// アニメーションの長さ（ticks単位）を設定します。
	/// </summary>
	/// <param name="duration"></param>
	void SetDuration(float duration) { m_duration = duration; }

	/// <summary>
	/// 1秒あたりのtick数を設定します。
	/// </summary>
	/// <param name="ticks"></param>
	void SetTicksPerSecond(float ticks) { m_ticksPerSecond = ticks; }

	/// <summary>
	/// ボーンアニメーションデータを変更可能な形で取得します。
	/// </summary>
	/// <returns></returns>
	std::unordered_map<std::string, BoneAnimation>& GetMutableBoneAnimations() { return m_boneAnimations; }

	/// <summary>
	/// ボーンアニメーションデータを読み取り専用で取得します。
	/// </summary>
	/// <returns></returns>
	const std::unordered_map<std::string, BoneAnimation>& GetBoneAnimations() const { return m_boneAnimations; }
};