#include "Precompiled.h"
#include "Path.h"

void AssetImporter::SetAssetPath(const wchar_t* path)
{
	m_assetPath = path;
	SplitAssetPath();
}

void AssetImporter::SplitAssetPath()
{
    const std::wstring& assetPath = GetAssetPath();
    m_directoryName = Path::GetDirectoryName(assetPath);
    m_fileName = Path::GetFileName(assetPath);
    m_fileNameWithoutExtension = Path::GetFileNameWithoutExtension(m_fileName);
    m_extension = Path::GetExtension(m_fileName);
}
