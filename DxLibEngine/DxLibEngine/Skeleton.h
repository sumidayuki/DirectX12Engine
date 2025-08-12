#pragma once

// 各ボーンの情報
struct BoneInfo
{
	int id;						// ボーンのインデックス
	Matrix4x4 offsetMatrix;		// Tポーズからボーンのローカル空間への変換行列
};

// ボーンの階層構造
struct Bone
{
	std::string name;
	Matrix4x4 localTransform;
	std::vector<Bone> children;
};

class Skeleton : public Reference
{
private:
	Bone m_rootBone;
	std::unordered_map<std::string, BoneInfo> m_boneInfoMap;
	int m_boneCount;
};