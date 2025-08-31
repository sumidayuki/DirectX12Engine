#pragma once

// �f�B�X�N���v�^�q�[�v�̎��
enum class DescriptorHeapType
{
	CBV_SRV_UAV,
	RTV,
	DSV,
	SAMPLER,
};

class DescriptorHeap : public Reference
{
private:
	DescriptorHeapType				m_heapType;				// �f�B�X�N���v�^�q�[�v�̎��
	int								m_stride;				// �f�B�X�N���v�^1���̃T�C�Y�i�P�ʂ̓o�C�g�j
	int								m_count;				// �v�f��
	bool							m_isShaderVisible;		// �V�F�[�_�̉���
	ComPtr<ID3D12DescriptorHeap>	m_nativeHeap;			// Direct3D12�̃C���^�[�t�F�C�X

public:
	// �R���X�g���N�^
	DescriptorHeap(DescriptorHeapType heapType, int count, bool isShaderVisible);

	/// <summary>
	/// �f�B�X�N���v�^�q�[�v�̎�ނ�\���l���擾���܂��B
	/// </summary>
	/// <returns>DescriptorHeapType�^�̒l��Ԃ��܂��B</returns>
	DescriptorHeapType GetHeapType() const { return m_heapType; }

	/// <summary>
	/// �f�B�X�N���v�^1���̃T�C�Y���擾���܂��B�i�P�ʂ̓o�C�g�j
	/// </summary>
	/// <returns>int�^�̒l��Ԃ��܂��B</returns>
	int GetStride() const { return m_stride; }

	/// <summary>
	/// �v�f�����擾���܂��B
	/// </summary>
	/// <returns>int�^�̒l��Ԃ��܂��B</returns>
	int GetCount() const { return m_count; }

	/// <summary>
	/// �V�F�[�_�̉������擾���܂��B
	/// </summary>
	/// <returns>bool�^�̒l��Ԃ��܂��B</returns>
	bool IsShaderVisible() const { return m_isShaderVisible; }

	/// <summary>
	/// Direct3D12�̃f�B�X�N���v�^�q�[�v�C���^�[�t�F�C�X���擾���܂��B
	/// </summary>
	/// <returns>ID3D12DescriptorHeap�^�̃|�C���^�[��Ԃ��܂��B</returns>
	ID3D12DescriptorHeap* GetNativeHeapPointer() const { return m_nativeHeap.Get(); }

	/// <summary>
	/// �q�[�v�擪����f�B�X�N���v�^index�������i�񂾏ꏊ��Ԃ��܂��B(CPU��)
	/// </summary>
	/// <param name="index"></param>
	/// <returns></returns>
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(int index) const;
	
	/// <summary>
	/// �q�[�v�擪����f�B�X�N���v�^index�������i�񂾏ꏊ��Ԃ��܂��B(GPU��)
	/// </summary>
	/// <param name="index"></param>
	/// <returns></returns>
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(int index) const;
};