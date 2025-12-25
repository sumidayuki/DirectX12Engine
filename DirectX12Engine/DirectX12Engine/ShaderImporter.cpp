#include "ShaderImporter.h"

std::string ReadAllText(const std::wstring& path)
{
	std::ifstream ifs(path, std::ios::binary);
	if (!ifs) return {};

	return std::string((std::istreambuf_iterator<char>(ifs)),
		std::istreambuf_iterator<char>());
}

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
		std::string text = ReadAllText(path);

		// debug: 最初の数文字確認
		OutputDebugStringA(text.substr(0, 20).c_str());

		Json json = Json::parse(text);

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
		std::ostringstream oss;
		oss << "JSON要素の解析中にエラーが発生: " << e.what() << "\n";
		OutputDebugStringA(oss.str().c_str());
	}
	catch (const Json::type_error& e)
	{
		std::ostringstream oss;
		oss << "JSON要素の解析中にエラーが発生: " << e.what() << "\n";
		OutputDebugStringA(oss.str().c_str());
	}
	catch (const Json::other_error& e)
	{
		std::ostringstream oss;
		oss << "JSON要素の解析中にエラーが発生: " << e.what() << "\n";
		OutputDebugStringA(oss.str().c_str());
	}
	catch (const Json::exception& e)
	{
		std::ostringstream oss;
		oss << "JSON要素の解析中にエラーが発生: " << e.what() << "\n";
		OutputDebugStringA(oss.str().c_str());
	}

	return new Shader(info);
}

void ShaderImporter::ProcessPath(const Json& json, ShaderInfo& info)
{
	info.hlslPath = UTF8toUTF16LE::Convert(json.at("HLSLPath").get<std::string>());
	info.vsEntry = json.at("VS_Entry").get<std::string>();
	info.vsShaderModel = json.at("VS_ShaderModel").get<std::string>();
	info.psEntry = json.at("PS_Entry").get<std::string>();
	info.psShaderModel = json.at("PS_ShaderModel").get<std::string>();

	OutputDebugStringW((L"Shaderのパス読み込みに成功\n"));
}

void ShaderImporter::ProcessInputLayout(const Json& json, ShaderInfo& info)
{
	// 配列全体を取得（キーが見つからない場合は例外がスローされます）
	const Json& array = json.at("InputLayout");

	for (const auto& element : array)
	{
		// 各JSONオブジェクトから "Semantic" と "Format" の値を取得(値の型が一致しない場合に例外がスローされます)
		InputElement inputElement;
		inputElement.Semantic = element.at("Semantic").get<std::string>();
		inputElement.Format = element.at("Format").get<std::string>();
		info.inputLayout.push_back(inputElement);
	}

	OutputDebugStringW((L"ShaderのInputLayout読み込みに成功\n"));
}

void ShaderImporter::ProcessPrimitiveTopology(const Json& json, ShaderInfo& info)
{
	info.primitiveTopology = json.value("PrimitiveTopology", "Triangle");
	OutputDebugStringW((L"ShaderのPrimitiveTopology読み込みに成功\n"));
}

void ShaderImporter::ProcessRasterizer(const Json& json, ShaderInfo& info)
{
	const Json& config = json.value("Rasterizer", Json::object());
	RasterizerInfo rasterizerInfo;
	rasterizerInfo.FillMode = config.value("FillMode", "Solid");
	rasterizerInfo.CullMode = config.value("CullMode", "Back");
	rasterizerInfo.FrontCCW = config.value("FrontCCW", false);
	info.rasterizer = rasterizerInfo;
	OutputDebugStringW((L"ShaderのRasteraizer読み込みに成功\n"));
}

void ShaderImporter::ProcessDepth(const Json& json, ShaderInfo& info)
{
	const Json& config = json.value("Depth", Json::object());
	DepthInfo depthInfo;
	depthInfo.Enable = config.value("Enable", true);
	depthInfo.WriteMask = config.value("WriteMask", "ALL");
	depthInfo.Func = config.value("Func", "Less");
	info.depth = depthInfo;
	OutputDebugStringW((L"ShaderのDepth読み込みに成功\n"));
}

void ShaderImporter::ProcessBlend(const Json& json, ShaderInfo& info)
{
	const Json& config = json.value("Blend", Json::object());
	BlendInfo blendInfo;
	blendInfo.Enable = config.value("Enable", false);
	blendInfo.Src = config.value("Src", "SRC_ALPHA");
	blendInfo.Dst = config.value("Dst", "INV_SRC_ALPHA");
	blendInfo.Op = config.value("Op", "ADD");
	info.blend = blendInfo;
	OutputDebugStringW((L"ShaderのBlend読み込みに成功\n"));
}

void ShaderImporter::ProcessRTVFormat(const Json& json, ShaderInfo& info)
{
	info.rtvFormatString = json.value("RTVFormat", "R8G8B8A8_UNORM");
	OutputDebugStringW((L"ShaderのRTVFormat読み込みに成功\n"));
}

void ShaderImporter::ProcessDSVFormat(const Json& json, ShaderInfo& info)
{
	info.dsvFormatString = json.value("DSVFormat", "D32_FLOAT");
	OutputDebugStringW((L"ShaderのDSVFormat読み込みに成功\n"));
}
