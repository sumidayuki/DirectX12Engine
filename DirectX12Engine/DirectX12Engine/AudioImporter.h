#pragma once

/// <summary>
/// オーディオをインポートするクラスです。
/// Wavファイルに対応しています。
/// </summary>
class AudioImporter : public AssetImporter
{
public:
	AudioImporter() = default;
	virtual ~AudioImporter() override = default;

	AudioClip* Import(const std::wstring& path);
	AudioClip* Import(const wchar_t* path);
	AudioClip* Import();

private:
	bool ParseWavFile(const wchar_t* path, WAVEFORMATEX& outFormat, std::vector<BYTE>& outData);
};