#pragma once

// �e�{�[���̏��
struct BoneInfo
{
	int id;						// �{�[���̃C���f�b�N�X
	Matrix4x4 offsetMatrix;		// T�|�[�Y����{�[���̃��[�J����Ԃւ̕ϊ��s��
};

// �{�[���̊K�w�\��
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