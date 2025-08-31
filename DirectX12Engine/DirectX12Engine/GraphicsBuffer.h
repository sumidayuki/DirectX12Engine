#pragma once

/// <summary>
/// CPU����A�N�Z�X�\�ȃ�������ɋL���̈���쐬���ă����_�����O�ɕK�v�ȃf�[�^���i�[���܂��B
/// ���̃O���t�B�b�N�X�o�b�t�@�́u�A�b�v���[�h�o�b�t�@�v�u�f�t�H���g�o�b�t�@�v�����̋@�\������Ă��܂��B
/// </summary>
class GraphicsBuffer : public Reference
{
public:
	// �O���t�B�b�N�X�o�b�t�@�̗p�r��\���񋓌^
	enum class Target
	{
		Vertex,
		Index,
		Constant,
		Structured,
	};

	// �O���t�B�b�N�X�o�b�t�@�̎g�p���@��\���񋓌^
	enum class UsageFlags
	{
		None,
		LockBufferForWrite,
	};

private:
	// �����ȃC���f�b�N�X
	static constexpr int InvalidIndex = -1;

private:
	Target					m_target;
	UsageFlags				m_usageFlags;
	int						m_count;
	int						m_stride;
	int						m_sizeInBytes;
	D3D12_RESOURCE_STATES	m_nativeBufferOriginallyState;
	ComPtr<ID3D12Resource>	m_nativeBuffer;
	ComPtr<ID3D12Resource>	m_uploadBuffer;
	int						m_lockStartIndex;
	int						m_lookedCount;

private:
	/// <summary>
	/// �f�t�H���g�o�b�t�@���쐬���܂��B
	/// </summary>
	/// <param name="size"></param>
	/// <param name="initialState"></param>
	/// <returns></returns>
	static ID3D12Resource* CreateDefaultBuffer(int size, D3D12_RESOURCE_STATES initialState);

	/// <summary>
	/// �A�b�v���[�h�o�b�t�@���쐬���܂�
	/// </summary>
	/// <param name="size"></param>
	/// <returns></returns>
	static ID3D12Resource* CreateUploadBuffer(int size);

	/// <summary>
	/// �p�r�A�g�p���@�ɍ��v���郊�\�[�X��Ԃ��擾���܂��B
	/// </summary>
	/// <param name="target"></param>
	/// <param name="usageFlags"></param>
	/// <returns></returns>
	static D3D12_RESOURCE_STATES GetOriginallyResourceState(Target target, UsageFlags usageFlags);

	/// <summary>
	/// �w�肵���̈�����b�N���ď������ݗp�̃A�h���X��Ԃ��܂��B
	/// </summary>
	/// <param name="bufferStartIndex"></param>
	/// <param name="count"></param>
	/// <returns></returns>
	void* InternalLockBufferForWrite(int bufferStartIndex, int count);

	/// <summary>
	/// ���b�N���Ă����̈���A�����b�N���āA�ύX�����r�f�I�������ɓ]�����܂��B
	/// </summary>
	/// <param name="countWritten"></param>
	void InternalUnlockBufferAfterWrite(int countWritten);

public:
	/// <summary>
	/// �O���t�B�b�N�X�o�b�t�@���쐬���܂��B
	/// </summary>
	/// <param name="target"></param>
	/// <param name="usageFlags"></param>
	/// <param name="count"></param>
	/// <param name="stride"></param>
	/// <param name="initialData"></param>
	GraphicsBuffer(Target target, UsageFlags usageFlags, int count, int stride, const void* initialData = nullptr);

	/// <summary>
	/// ���̃O���t�B�b�N�X�o�b�t�@�̗p�r��\���l���擾���܂��B
	/// </summary>
	/// <returns></returns>
	Target GetTarget() const { return m_target; }

	/// <summary>
	/// ���̃O���t�B�b�N�X�o�b�t�@�̎g�p���@��\���l���擾���܂��B
	/// </summary>
	/// <returns></returns>
	UsageFlags GetUsageFlags() const { return m_usageFlags; }

	/// <summary>
	/// �v�f�����擾���܂��B
	/// </summary>
	/// <returns></returns>
	int GetCount() const { return m_count; }

	/// <summary>
	/// �v�f1���̃o�C�g�����擾���܂��B
	/// </summary>
	/// <returns></returns>
	int GetStride() const { return m_stride; }

	/// <summary>
	/// �o�b�t�@�S�̂̃o�C�g�����擾���܂��B
	/// </summary>
	/// <returns></returns>
	int GetSizeInBytes() const { return m_sizeInBytes; }

	/// <summary>
	/// �o�b�t�@���\�[�X�̃C���^�[�t�F�C�X���擾���܂��B
	/// </summary>
	/// <returns>ID3D12Resource�^�|�C���^�̒l��Ԃ��܂��B</returns>
	ID3D12Resource* GetNativeBufferPtr() const { return m_nativeBuffer.Get(); }

	/// <summary>
	/// UsageFlags::LockBufferForWrite�t���O�����ō쐬���ꂽ�o�b�t�@���X�V���邱�Ƃ��ł��܂��B
	/// </summary>
	/// <param name="bufferStartIndex"></param>
	/// <param name="count"></param>
	/// <param name="data"></param>
	void UpdateSubresource(int bufferStartIndex, int count, const void* data);

	/// <summary>
	/// �o�b�t�@���̎w�肵���̈�����b�N���ď������ݗp�̃A�h���X��Ԃ��܂��B
	/// </summary>
	/// <param name="bufferStartIndex"></param>
	/// <param name="count"></param>
	/// <returns></returns>
	[[nodiscard]] void* LockBufferForWrite(int bufferStartIndex, int count);

	/// <summary>
	/// �o�b�t�@�S�̂����b�N���ď������ݗp�̃A�h���X��Ԃ��܂��B
	/// </summary>
	/// <returns></returns>
	[[nodiscard]] void* LockBufferForWrite() { return LockBufferForWrite(0, m_count); }

	/// <summary>
	/// ���b�N���Ă����̈���A�����b�N���܂��B
	/// </summary>
	/// <param name="countWritten"></param>
	void UnlockBufferAfterWrite(int countWritten);

	/// <summary>
	/// ���b�N���Ă����̈���A�����b�N���A�����b�N���܂��B
	/// </summary>
	void UnlockBufferAfterWrite() { UnlockBufferAfterWrite(m_lookedCount); }
};