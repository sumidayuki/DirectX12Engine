#include "ShaderBytecode.h"

// ���C�u�����������N����
#pragma comment(lib, "d3dcompiler.lib")

ShaderBytecode::ShaderBytecode(const wchar_t* path, const char* entryPointName, const char* shaderModel)
{
	ComPtr<ID3DBlob> errorMessage;

	// �e�L�X�g�t�@�C���i.hlsl�j�ɋL�q���ꂽ�V�F�[�_�[�R�[�h���R���p�C�����ăo�C�i���R�[�h�ɕϊ����܂��B
	if (D3DCompileFromFile(path, nullptr, nullptr, entryPointName, shaderModel, 0, 0, m_bytecode.ReleaseAndGetAddressOf(), errorMessage.ReleaseAndGetAddressOf()) < 0)
	{
		// �R���p�C���G���[�����񂪂���ꍇ�́AVisual Studio�́u�o�́v�E�B���h�E�ɕ\������
		if (errorMessage)
		{
			OutputDebugStringA((const char*)errorMessage->GetBufferPointer());
		}

		// ����ȏ㑱���Ă��G���[���A���������邾���Ȃ̂Ńv���O�����𒆒f����B
		assert(0);
	}
}
