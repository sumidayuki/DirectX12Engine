#include "Precompiled.h"

void AssetImporter::SetAssetPath(const wchar_t* path)
{
	m_assetPath = path;
	SplitAssetPath();
}

void AssetImporter::SplitAssetPath()
{
	const std::wstring& assetPath = GetAssetPath();
}
