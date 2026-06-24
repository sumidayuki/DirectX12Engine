#include "AudioImporter.h"

AudioClip* AudioImporter::Import(const std::wstring& path)
{
	SetAssetPath(path.c_str());
	return Import();
}

AudioClip* AudioImporter::Import(const wchar_t* path)
{
	SetAssetPath(path);
	return Import();
}

AudioClip* AudioImporter::Import()
{
	const std::wstring& path = GetAssetPath();
	if (path.empty())
	{
		OutputDebugStringW(L"AudioImporter Error: Asset path is empty.\n");
		return nullptr;
	}
	WAVEFORMATEX format;
	std::vector<BYTE> audioData;
	if (!ParseWavFile(path.c_str(), format, audioData))
	{
		OutputDebugStringW((L"AudioImporter Error: Failed to parse WAV file at " + path + L"\n").c_str());
		return nullptr;
	}
	int lengthSamples = static_cast<int>(audioData.size() / (format.nChannels * (format.wBitsPerSample / 8)));
	int channels = format.nChannels;
	int frequency = format.nSamplesPerSec;
	std::string name =  UTF16LEtoUTF8::Convert(GetFileNameWithoutExtension());

	return AudioClip::Create(name, format, audioData, lengthSamples, channels, frequency);
}

bool AudioImporter::ParseWavFile(const wchar_t* path, WAVEFORMATEX& outFormat, std::vector<BYTE>& outData)
{
	FILE* file = nullptr;
	_wfopen_s(&file, path, L"rb");
	if (!file)
	{
		OutputDebugStringW((L"AudioImporter Error: Failed to open file at " + std::wstring(path) + L"\n").c_str());
		return false;
	}
	char chunkID[4];
	DWORD chunkSize;
	char format[4];
	fread(chunkID, sizeof(char), 4, file);
	fread(&chunkSize, sizeof(DWORD), 1, file);
	fread(format, sizeof(char), 4, file);
	if (strncmp(chunkID, "RIFF", 4) != 0 || strncmp(format, "WAVE", 4) != 0)
	{
		OutputDebugStringW((L"AudioImporter Error: Invalid WAV file format at " + std::wstring(path) + L"\n").c_str());
		fclose(file);
		return false;
	}
	while (fread(chunkID, sizeof(char), 4, file) == 4)
	{
		fread(&chunkSize, sizeof(DWORD), 1, file);
		if (strncmp(chunkID, "fmt ", 4) == 0)
		{
			WAVEFORMATEX fmt;
			fread(&fmt.wFormatTag, sizeof(WORD), 1, file);
			fread(&fmt.nChannels, sizeof(WORD), 1, file);
			fread(&fmt.nSamplesPerSec, sizeof(DWORD), 1, file);
			fread(&fmt.nAvgBytesPerSec, sizeof(DWORD), 1, file);
			fread(&fmt.nBlockAlign, sizeof(WORD), 1, file);
			fread(&fmt.wBitsPerSample, sizeof(WORD), 1, file);
			outFormat = fmt;
			fseek(file, chunkSize - sizeof(WAVEFORMATEX) + sizeof(WORD), SEEK_CUR);
		}
		else if (strncmp(chunkID, "data", 4) == 0)
		{
			outData.resize(chunkSize);
			fread(outData.data(), sizeof(BYTE), chunkSize, file);
			break; // データチャンクを読み込んだら終了
		}
		else
		{
			fseek(file, chunkSize, SEEK_CUR); // 不要なチャンクはスキップ
		}
	}
	fclose(file);
	return true;
}