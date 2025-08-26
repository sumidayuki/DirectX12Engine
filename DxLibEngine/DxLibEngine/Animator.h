#pragma once

/// <summary>
/// �G���e�B�e�B�̃A�j���[�V������Ԃ��Ǘ�����R���|�[�l���g�ł��B
/// </summary>
struct Animator
{
	// �Đ�����A�j���[�V�����N���b�v
	Animation* animation = nullptr;

	// ���݂̍Đ�����
	float currentTime = 0.0f;

	// �A�j���[�V�������Đ������ǂ���
	bool isPlaying = true;

	// �A�j���[�V���������[�v�����邩
	bool isLoop = true;

	// �{�[���̍ŏI�I�ȕϊ��s��
	// AnimationSystem�����t���[���v�Z���ASkinnedMeshRendererSystem���g�p���܂��B
	std::vector<Matrix4x4> finalBoneMatrices;
};