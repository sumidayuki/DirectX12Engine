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

struct Animation
{
	std::string name;
	float duration;
	float ticksPerSecond;
	std::unordered_map<std::string, BoneAnimation> boneAnimations;
};