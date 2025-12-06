#pragma once

/// <summary>
/// Root Signature、HLSLバイトコード、PSOテンプレートを持つ
/// Shaderアセットと、動的な状態に応じたID3D12PipelineState (PSO) を
/// 管理・キャッシュするレジストリクラス。
/// </summary>
class ShaderRegistry
{
private:
	static inline ComPtr<ID3D12RootSignature> m_rootSignature;

	// パスとShaderオブジェクトのマップ
	static inline std::unordered_map<std::string, ComPtr<Shader>> m_shaderCache;

private:
	static void StaticConstructor();
	static void StaticDestructor();
	static void LoadShader(const std::wstring& path);
	static void AllShadersCompile();

public:
	static const Shader* GetShader(const std::string& path);
};