#pragma once

/// <summary>
/// �N���X���FAssetImporter
/// �T�v�F�O������f�ނ�ǂݍ��ލۂ̋��ʕ������Ǘ�������N���X�ł��B
/// �p�r�F���̃N���X���p�����邱�Ńt�@�C���̃p�X������Ȃǂ��Ǘ������Ƃ��ł��܂��B
/// </summary>
class AssetImporter
{
private:
	std::wstring m_assetPath;					// �A�Z�b�g�p�X�F�A�Z�b�g�t�@�C���ւ̃p�X�������\���܂��B
	std::wstring m_directoryName;				// �f�B���N�g��
	std::wstring m_fileName;					// �t�@�C����
	std::wstring m_fileNameWithoutExtension;	// �t�@�C����
	std::wstring m_extension;					// �g���q
	std::wstring m_userData;					// ���[�U�[�f�[�^

protected:
	// �f�t�H���g�R���X�g���N�^
	AssetImporter() = default;

	// ���z�f�X�g���N�^
	virtual ~AssetImporter() = default;

public:
	/// <summary>
	/// �A�Z�b�g�t�@�C���ւ̃p�X�������ݒ肵�܂��B
	/// </summary>
	/// <param name="path">L"������"�̌`�Ń\�����[�V�����t�H���_����̃p�X���w�肵�Ă��������B</param>
	void SetAssetPath(const wchar_t* path);

	/// <summary>
	/// �A�Z�b�g�t�@�C���ւ̃p�X��������擾���܂��B
	/// </summary>
	/// <returns>std::wstring�^��path��Ԃ��܂��B</returns>
	const std::wstring& GetAssetPath() const { return m_assetPath; }

	/// <summary>
	/// �A�Z�b�g�t�@�C���̃f�B���N�g�������擾���܂��B
	/// </summary>
	/// <returns>std::wstring�^��path��Ԃ��܂��B</returns>
	const std::wstring& GetDirectoryName() const { return m_directoryName; }

	/// <summary>
	/// �A�Z�b�g�t�@�C���̃t�@�C�������擾���܂��B
	/// </summary>
	/// <returns>std::wstring�^��path��Ԃ��܂��B</returns>
	const std::wstring& GetFileName() const { return m_fileName; }

	/// <summary>
	/// �A�Z�b�g�t�@�C���̊g���q�Ȃ��̃t�@�C�������擾���܂��B
	/// </summary>
	/// <returns>std::wstring�^��path��Ԃ��܂��B</returns>
	const std::wstring& GetFileNameWithoutExtension() const { return m_fileNameWithoutExtension; }

	/// <summary>
	/// �A�Z�b�g�t�@�C���̊g���q���擾���܂��B
	/// </summary>
	/// <returns>std::wstring�^��path��Ԃ��܂��B</returns>
	const std::wstring& GetExtension() const { return m_extension; }

	/// <summary>
	/// �C�ӂ̃��[�U�[�������ݒ肵�܂��B
	/// </summary>
	/// <param name="userData"></param>
	void SetUserData(const wchar_t* userData) { m_userData = userData; }

	/// <summary>
	/// �C�ӂ̃��[�U�[��������擾���܂��B
	/// </summary>
	/// <returns></returns>
	const std::wstring& GetUserData() const { return m_userData; }

private:
	void SplitAssetPath();
};