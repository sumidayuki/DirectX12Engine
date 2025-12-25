#include "ShaderBytecode.h"

// ライブラリをリンクする
#pragma comment(lib, "d3dcompiler.lib")

ShaderBytecode::ShaderBytecode(const wchar_t* path, const char* entryPointName, const char* shaderModel)
{
	ComPtr<ID3DBlob> errorMessage;

	// テキストファイル（.hlsl）に記述されたシェーダーコードをコンパイルしてバイナリコードに変換します。
	if (D3DCompileFromFile(path, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPointName, shaderModel, 0, 0, m_bytecode.ReleaseAndGetAddressOf(), errorMessage.ReleaseAndGetAddressOf()) < 0)
	{
		// コンパイルエラー文字列がある場合は、Visual Studioの「出力」ウィンドウに表示する
		if (errorMessage)
		{
			OutputDebugStringA((const char*)errorMessage->GetBufferPointer());
		}

		// それ以上続けてもエラーが連続発生するだけなのでプログラムを中断する。
		assert(0);
	}
}

ShaderBytecode::ShaderBytecode(const wchar_t* path, const char* entryPointName, const char* shaderModel, const D3D_SHADER_MACRO* defines)
{
	ComPtr<ID3DBlob> errorMessage;

	if (D3DCompileFromFile(path, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPointName, shaderModel, 0, 0, m_bytecode.ReleaseAndGetAddressOf(), errorMessage.ReleaseAndGetAddressOf()) < 0)
	{
		if (errorMessage)
		{
			OutputDebugStringA((const char*)errorMessage->GetBufferPointer());
		}
		assert(0);
	}
}
