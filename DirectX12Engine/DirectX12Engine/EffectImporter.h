#pragma once

class EffectImporter : public AssetImporter
{
public:
	EffectImporter() = default;
	virtual ~EffectImporter() override = default;

	Effect* Import(const std::wstring& path);
	Effect* Import(const wchar_t* path);
	Effect* Import();
};