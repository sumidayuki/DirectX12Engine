#include "ShaderImporter.h"

ShaderImporter::ShaderImporter()
{
}

Shader* ShaderImporter::Import(const std::wstring path)
{
	SetAssetPath(path.c_str());

	return Import();
}

Shader* ShaderImporter::Import(const wchar_t* path)
{
	SetAssetPath(path);

	return Import();
}

Shader* ShaderImporter::Import()
{
	// パスを取得
	const std::wstring& path = GetAssetPath();
	
	// パスが空なら無効
	if (path.empty())
	{
		return nullptr;
	}

	ShaderInfo info;

	try
	{
		// Json全体をパースしてルートオブジェクトを取得
		Json json = Json::parse(path);

		ProcessPath(json, info);
		ProcessInputLayout(json, info);
		ProcessPrimitiveTopology(json, info);
		ProcessRasterizer(json, info);
		ProcessDepth(json, info);
		ProcessBlend(json, info);
		ProcessRTVFormat(json, info);
		ProcessDSVFormat(json, info);
	}
	catch (const Json::parse_error& e)
	{
		assert(0);
	}
	catch (const Json::type_error& e)
	{
		assert(0);
	}
	catch (const Json::other_error& e)
	{
		assert(0);
	}

	return new Shader(info);
}

void ShaderImporter::ProcessPath(const Json& json, ShaderInfo& info)
{
	// パスキーが存在していてかつパスが文字列で書かれているか確認します。
	if (json.contains("HLSLPath") && json["HLSLPath"].is_string())
	{
		info.hlslPath = json["HLSLPath"];
	}
	else
	{
		assert(0);
	}

	if (json.contains("VS_Entry") && json["VS_Entry"].is_string())
	{
		info.vsEntry = json["VS_Entry"];
	}
	else
	{
		assert(0);
	}

	if (json.contains("VS_ShaderModel") && json["VS_ShaderModel"].is_string())
	{
		info.vsShaderModel = json["VS_ShaderModel"];
	}
	else
	{
		assert(0);
	}

	if (json.contains("PS_Entry") && json["PS_Entry"].is_string())
	{
		info.psEntry = json["PS_Entry"];
	}
	else
	{
		assert(0);
	}

	if (json.contains("PS_ShaderModel") && json["PS_ShaderModel"].is_string())
	{
		info.psShaderModel = json["PS_ShaderModel"];
	}
	else
	{
		assert(0);
	}
}

void ShaderImporter::ProcessInputLayout(const Json& json, ShaderInfo& info)
{
	for (int i = 0; i < json["InputLayout"].size(); i++)
	{
		json["InputLayout"][i];
	}
	
}

void ShaderImporter::ProcessPrimitiveTopology(const Json& json, ShaderInfo& info)
{
}

void ShaderImporter::ProcessRasterizer(const Json& json, ShaderInfo& info)
{
}

void ShaderImporter::ProcessDepth(const Json& json, ShaderInfo& info)
{
}

void ShaderImporter::ProcessBlend(const Json& json, ShaderInfo& info)
{
}

void ShaderImporter::ProcessRTVFormat(const Json& json, ShaderInfo& info)
{
}

void ShaderImporter::ProcessDSVFormat(const Json& json, ShaderInfo& info)
{
}
