#pragma once

/// <summary>
/// �Q�ƃJ�E���g�����̃������Ǘ��N���X�ł��B
/// ���̃I�u�W�F�N�g���Q�Ƃ��Ă�����̂�1�ȏ㑶�݂���ꍇ�A���̃I�u�W�F�N�g�̓������Ɏc�葱���܂��B
/// �Q�ƃJ�E���g�� 0 �ɂȂ����Ƃ��A�����I�Ɏ��g�� delete ���ă���������������܂��B
/// 
/// IUnknown ���p�����邱�Ƃ� COM �̏����̃C���^�[�t�F�C�X�����悤�ɂȂ�A
/// COM�I�u�W�F�N�g�Ɠ��l�Ɉ������Ƃ��ł��܂��B�iAddRef�ARelease�AQueryInterFace�̒񋟁j
/// </summary>
class Reference : IUnknown	
{
private:
	// �Q�ƃJ�E���g
	ULONG m_referenceCount;

protected:
	// �f�t�H���g�R���X�g���N�^
	Reference();

	// ���z�f�X�g���N�^
	virtual ~Reference() = default;

public:
	/// <summary>
	/// �Q�ƃJ�E���g���擾���܂��B
	/// </summary>
	/// <returns></returns>
	ULONG GetReferenceCount() const { return m_referenceCount; }

	/// <summary>
	/// �Q�ƃJ�E���g�� 1 ���₵�܂��B
	/// </summary>
	/// <returns></returns>
	ULONG AddRef() override { return ++m_referenceCount; }

	/// <summary>
	/// �Q�ƃJ�E���g�� 1 ���炵�A0�ɂȂ����ꍇ�͂��̃C���X�^���X�� delete ���܂��B
	/// </summary>
	/// <returns></returns>
	ULONG Release() override;

	/// <summary>
	/// �C���^�[�t�F�C�XID���w�肵�āA�C���^�[�t�F�C�X���擾���܂��B
	/// </summary>
	/// <param name="interfaceID"></param>
	/// <param name="ppInterface"></param>
	/// <returns></returns>
	HRESULT QueryInterface(const IID& interfaceID, void** ppInterface) override;
};