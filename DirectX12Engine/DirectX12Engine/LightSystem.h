#pragma once

class LightSystem : public System
{
private:
	static const int MAX_LIGHT = 32;

private:
	ComPtr<GraphicsBuffer> m_lightBuffer;					// �S�Ẵ��C�g�����i�[����\�����o�b�t�@
	D3D12_GPU_DESCRIPTOR_HANDLE m_lightBufferGpuHandle;		// m_lightBuffer��SRV���i�[����f�X�N���v�^�q�[�v

	int m_activeLightCount = 0; // ���ݗL���ȃ��C�g�̐�

public:
	/// <summary>
	/// ���ݗL���ȃ��C�g�̐����擾���܂��B
	/// </summary>
	int GetActiveLightCount() const { return m_activeLightCount; }

	/// <summary>
	/// ���̃V�X�e�����Ǘ�����f�X�N���v�^�q�[�v���擾���܂��B
	/// �iMeshRendererSystem��SetDescriptorHeaps�Őݒ肷�邽�߂Ɏg�p�j
	/// </summary>
	D3D12_GPU_DESCRIPTOR_HANDLE GetLightBufferGpuHandle() const { return m_lightBufferGpuHandle; }

private:
	/// <summary>
	/// �V�X�e���̏����������B���\�[�X�̍쐬�Ȃǂ��s���܂��B
	/// </summary>
	void Start(ComponentManager& cm, World& world) override;

	/// <summary>
	/// ���t���[���̍X�V�����B�V�[�����̃��C�g�������W���A�o�b�t�@���X�V���܂��B
	/// </summary>
	void Update(ComponentManager& cm, World& world) override;
};