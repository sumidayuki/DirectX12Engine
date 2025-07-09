#include "ShaderBytecode.h"

// ライブラリをリンクする
#pragma comment(lib, "d3dcompiler.lib")

ShaderBytecode::ShaderBytecode(const wchar_t* path, const char* entryPointName, const char* shaderModel)
{
	ComPtr<ID3DBlob> errorMessage;

	// テキストファイル（.hlsl）に記述されたシェーダーコードをコンパイルしてバイナリコードに変換します。
	if (D3DCompileFromFile(path, nullptr, nullptr, entryPointName, shaderModel, 0, 0, m_bytecode.ReleaseAndGetAddressOf(), errorMessage.ReleaseAndGetAddressOf()) < 0)
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
