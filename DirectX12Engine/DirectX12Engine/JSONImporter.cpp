#include "JSONImporter.h"

Json JSONImporter::Import(const std::wstring& path)
{
	SetAssetPath(path.c_str());
	return Import();
}

Json JSONImporter::Import(const wchar_t* path)
{
	SetAssetPath(path);
	return Import();
}

Json JSONImporter::Import()
{
	std::ifstream file(GetAssetPath());

	if (!file.is_open())
	{
		assert(0);
	}

	try
	{
		std::string text = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

		Json json = Json::parse(text, nullptr, true, true);
		return json;
	}
	catch (const Json::exception& e)
	{
		std::ostringstream oss;

		oss << "JSON要素の解析中にエラーが発生: " << e.what() << "\n";

#if _DEBUG
		OutputDebugStringA(oss.str().c_str());
		assert(0);
#endif
	}

	return nullptr;
}