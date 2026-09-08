#pragma once

class JSONImporter : public AssetImporter
{
public:
	JSONImporter() = default;
	virtual ~JSONImporter() override = default;

	Json Import(const std::wstring& path);
	Json Import(const wchar_t* path);
	Json Import();
};