#pragma once

class ShaderBytecode : public Reference
{
private:
	// コンパイル済みのシェーダーバイトコード
	ComPtr<ID3DBlob> m_bytecode;

public:
	/// <summary>
	/// HLSLで記述されたファイルをコンパイルしてバイトコードを作成します。
	/// </summary>
	/// <param name="path"></param>
	/// <param name="entryPointName"></param>
	/// <param name="shaderModel"></param>
	ShaderBytecode(const wchar_t* path, const char* entryPointName, const char* shaderModel);

	/// <summary>
	/// バイトコードのバイト数を取得します。
	/// </summary>
	/// <returns></returns>
	size_t GetBytecodeLength() const { return m_bytecode->GetBufferSize(); }

	/// <summary>
	/// バイトコードの先頭アドレスを取得します。
	/// </summary>
	/// <returns></returns>
	const void* GetBytecodePointer() const { return m_bytecode->GetBufferPointer(); }
};