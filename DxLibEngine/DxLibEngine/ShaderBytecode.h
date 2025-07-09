#pragma once

class ShaderBytecode : public Reference
{
private:
	// �R���p�C���ς݂̃V�F�[�_�[�o�C�g�R�[�h
	ComPtr<ID3DBlob> m_bytecode;

public:
	/// <summary>
	/// HLSL�ŋL�q���ꂽ�t�@�C�����R���p�C�����ăo�C�g�R�[�h���쐬���܂��B
	/// </summary>
	/// <param name="path"></param>
	/// <param name="entryPointName"></param>
	/// <param name="shaderModel"></param>
	ShaderBytecode(const wchar_t* path, const char* entryPointName, const char* shaderModel);

	/// <summary>
	/// �o�C�g�R�[�h�̃o�C�g�����擾���܂��B
	/// </summary>
	/// <returns></returns>
	size_t GetBytecodeLength() const { return m_bytecode->GetBufferSize(); }

	/// <summary>
	/// �o�C�g�R�[�h�̐擪�A�h���X���擾���܂��B
	/// </summary>
	/// <returns></returns>
	const void* GetBytecodePointer() const { return m_bytecode->GetBufferPointer(); }
};