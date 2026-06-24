#pragma once

class AudioClip : public Reference
{
private:
	std::string         m_name;
	WAVEFORMATEX        m_format;
	std::vector<BYTE>   m_audioData;
	float               m_length;
	int                 m_channels;
	int                 m_frequency;

private:
	// デフォルトコンストラクタ
	AudioClip();
	// 仮想デストラクタ
	virtual ~AudioClip() = default;

public:
	static AudioClip* Create(const std::string& name, const WAVEFORMATEX& format, const std::vector<BYTE>& audioData, int lengthSamples, int channels, int frequency);

	const std::string& GetName() const { return m_name; }
	 const WAVEFORMATEX& GetFormat() const { return m_format; }
	const std::vector<BYTE>& GetAudioData() const { return m_audioData; }
	const float GetLength() const { return m_length; }
	const int GetChannels() const { return m_channels; }
	const int GetFrequency() const { return m_frequency; }
};