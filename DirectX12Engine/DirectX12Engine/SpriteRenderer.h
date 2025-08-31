#pragma once

/// <summary>
/// �X�v���C�g�������_�����O���邽�߂̏������R���|�[�l���g�ł��B
/// �萔�o�b�t�@�ƃf�B�X�N���v�^�q�[�v���쐬����K�v������܂��B
/// </summary>
struct SpriteRenderer
{
	bool					isEnabled = true;
	ComPtr<Sprite>			sprite = nullptr;
	Color					color = Color::white;
	bool					flipX = false;
	bool					flipY = false;
	ComPtr<GraphicsBuffer>	constantBuffer = nullptr;
	ComPtr<DescriptorHeap>	descriptorHeap = nullptr;

	bool					isStarted = false;
};