#pragma once
#include <xaudio2.h>

class AudioManager : public Singleton<AudioManager>
{
	friend class Singleton<AudioManager>;

private:
	IXAudio2*               m_xaudio2;
	IXAudio2MasteringVoice* m_masteringVoice;
	float                   m_masterVolume;
	struct OneShotVoice
	{
		IXAudio2SourceVoice* voice;
		bool                 finished;
	};
	std::vector<OneShotVoice> m_oneShotVoices;

private:
	AudioManager();
	~AudioManager();

public:
	bool Initialize();
	void Shutdown();
	IXAudio2*               GetXAudio2()       const { return m_xaudio2; }
	IXAudio2MasteringVoice* GetMasteringVoice() const { return m_masteringVoice; }
	IXAudio2SourceVoice* CreateSourceVoice(const WAVEFORMATEX& format);
	void DestroySourceVoice(IXAudio2SourceVoice* voice);
	void  SetMasterVolume(float volume);
	float GetMasterVolume() const { return m_masterVolume; }
	void PlayOneShotInternal(AudioClip* clip, float volume);
	void CleanupFinishedVoices();
};