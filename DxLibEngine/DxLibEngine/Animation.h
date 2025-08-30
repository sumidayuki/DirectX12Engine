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
	std::string m_name;
	float m_duration;
	float m_ticksPerSecond;
	std::unordered_map<std::string, BoneAnimation> m_boneAnimations;

public:
	const std::string& GetName() const { return m_name; }
	float GetDuration() const { return m_duration; }
	float GetTicksPerSecond() const { return m_ticksPerSecond; }

	void SetName(const std::string& name) { m_name = name; }
	void SetDuration(float duration) { m_duration = duration; }
	void SetTicksPerSecond(float ticks) { m_ticksPerSecond = ticks; }

	std::unordered_map<std::string, BoneAnimation>& GetMutableBoneAnimations() { return m_boneAnimations; }

	const std::unordered_map<std::string, BoneAnimation>& GetBoneAnimations() const { return m_boneAnimations; }
};