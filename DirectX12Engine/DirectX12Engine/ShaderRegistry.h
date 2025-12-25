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
	static inline std::unordered_map<std::string, Shader*> m_shaderCache;

public:
	static void StaticConstructor();
	static void StaticDestructor();
	static void LoadShader(const std::wstring& path);
	static void AllShadersCompile();

public:
	static Shader* GetShader(const std::string& path);
	static ID3D12RootSignature* GetRootSignature() { return m_rootSignature.Get(); }
};