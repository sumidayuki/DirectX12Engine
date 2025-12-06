#pragma once

/// <summary>
/// Root Signature、HLSLバイトコード、PSOテンプレートを持つ
/// Shaderアセットと、動的な状態に応じたID3D12PipelineState (PSO) を
/// 管理・キャッシュするレジストリクラス。
/// </summary>
class ShaderRegistry
{
private:
	// パスとShaderオブジェクトのマップ
	static inline std::unordered_map<std::wstring, ComPtr<Shader>> m_shaderCache;

private:
	static void StaticConstructor();
	static void StaticDestructor();
	static void LoadShader(const std::wstring& path);
	static void AllShadersCompile();

public:
	static void GetShader(const std::wstring& path);
};