#pragma once

/// <summary>
/// �X�L�j���O���ꂽ���b�V���̃����_�����O�ɕK�v�ȏ���ێ�����R���|�[�l���g�B
/// �ÓI��MeshRenderer�Ƃ͋�ʂ���܂��B
/// </summary>
struct SkinnedMeshRenderer
{
	// �`�悷�郁�b�V��
	// ���̃��b�V���̓{�[�������܂�ł���K�v������܂��B
	ComPtr<Mesh> mesh = nullptr;

	// �`��Ɏg�p����}�e���A���Ƀ��X�g
	// Mesh�̃T�u���b�V���C���f�b�N�X�ƑΉ����܂��B
	std::vector<ComPtr<Material>> materials;

	// ���b�V���ɑΉ�����X�P���g��
	ComPtr<Skeleton> skeleton = nullptr;

	// ���̃����_���[�̃��[�g�{�[���ƂȂ�G���e�B�e�B
	// ����ɂ��A���f���̈ꕔ(���Ȃ�)��ʂ̃G���e�B�e�B�K�w�ɒǏ]�����邱�Ƃ��\�ɂȂ�܂��B
	// �ʏ�́A���̃R���|�[�l���g�����G���e�B�e�B���g�ł��B
	Entity rootBoneEntity;
};