#pragma once

// 各ボーンの情報
struct BoneInfo
{
	int id;						// ボーンのインデックス
	Matrix4x4 offsetMatrix;
};

// ボーンの階層構造
struct Bone
{
	std::string name;
	Matrix4x4 localTransform;
	std::vector<Bone*> children;
};

class Skeleton : public Reference
{
private:
	Bone* m_rootBone;
	Matrix4x4 m_globalInverseTransform;
	std::unordered_map<std::string, BoneInfo> m_boneInfoMap;
	int m_boneCount;

public:
	Bone* GetMutableRootBone() { return m_rootBone; }
	void SetRootBone(Bone* root) { m_rootBone = root; }

	std::unordered_map<std::string, BoneInfo>& GetMutableBoneInfoMap() { return m_boneInfoMap; }

	void SetBoneCount(int count) { m_boneCount = count; }

	void SetGlobalInverseTransform(const Matrix4x4& transform) { m_globalInverseTransform = transform; }

	const Bone* GetRootBone() const { return m_rootBone; }

	const std::unordered_map<std::string, BoneInfo>& GetBoneInfoMap() const { return m_boneInfoMap; }
	
	int GetBoneCount() const { return m_boneCount; }

	const Matrix4x4& GetGlobalInverseTransform() const { return m_globalInverseTransform; }
};