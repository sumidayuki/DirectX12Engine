#include "AudioClip.h"

AudioClip::AudioClip()
	 : m_name("")
	 , m_format({})
	 , m_audioData({})
	 , m_length(0.0f)
	 , m_channels(0)
	 , m_frequency(0)
{
}

AudioClip* AudioClip::Create(const std::string& name, const WAVEFORMATEX& format, const std::vector<BYTE>& audioData, int lengthSamples, int channels, int frequency)
{
	AudioClip* clip = new AudioClip();
	clip->m_name = name;
	clip->m_format = format;
	clip->m_audioData = audioData;
	clip->m_length = static_cast<float>(lengthSamples) / static_cast<float>(frequency);
	clip->m_channels = channels;
	clip->m_frequency = frequency;

	return clip;
}
