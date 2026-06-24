#include "AudioManager.h"

AudioManager::AudioManager()
     : m_xaudio2(nullptr)
     , m_masteringVoice(nullptr)
     , m_masterVolume(1.0f)
	 , m_oneShotVoices()
{
}

AudioManager::~AudioManager()
{
	Shutdown();
}

bool AudioManager::Initialize()
{
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE)
    {
        OutputDebugStringA("AudioManager Error: Failed to initialize COM library.\n");
		return false;
    }

	hr = XAudio2Create(&m_xaudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
    if (FAILED(hr))
    {
        OutputDebugStringA("AudioManager Error: Failed to create XAudio2 engine.\n");
        return false;
	}

	hr = m_xaudio2->CreateMasteringVoice(&m_masteringVoice);
    if (FAILED(hr))
    {
        OutputDebugStringA("AudioManager Error: Failed to create mastering voice.\n");
        m_xaudio2->Release();
        m_xaudio2 = nullptr;
        return false;
    }

    return true;
}

void AudioManager::Shutdown()
{
    for (auto& os : m_oneShotVoices)
    {
        if (os.voice)
        {
            os.voice->Stop(0);
            os.voice->DestroyVoice();
            os.voice = nullptr;
        }
    }

    m_oneShotVoices.clear();
    
    if (m_masteringVoice)
    {
        m_masteringVoice->DestroyVoice();
        m_masteringVoice = nullptr;
    }

    if (m_xaudio2)
    {
        m_xaudio2->Release();
        m_xaudio2 = nullptr;
    }

	CoUninitialize();
}

IXAudio2SourceVoice* AudioManager::CreateSourceVoice(const WAVEFORMATEX& format)
{
    if (!m_xaudio2)
    {
        return nullptr;
    }

    IXAudio2SourceVoice* sv = nullptr;
    HRESULT hr = m_xaudio2->CreateSourceVoice(&sv, &format);
    
    if (FAILED(hr))
    {
        OutputDebugStringA("AudioManager Error: Failed to create source voice.\n");
        return nullptr;
    }

	return sv;
}

void AudioManager::DestroySourceVoice(IXAudio2SourceVoice* voice)
{
    if (voice)
    {
        voice->Stop(0);
        voice->DestroyVoice();
	}
}

void AudioManager::SetMasterVolume(float volume)
{
    m_masterVolume = volume;
    if (m_masteringVoice)
    {
        m_masteringVoice->SetVolume(m_masterVolume);
	}
}

void AudioManager::PlayOneShotInternal(AudioClip* clip, float volume)
{
    if(!clip || clip->GetAudioData().empty())
    {
        return;
	}

	IXAudio2SourceVoice* voice = CreateSourceVoice(clip->GetFormat());
    if (!voice)
    {
        OutputDebugStringA("AudioManager Error: Failed to create source voice for one-shot playback.\n");
        return;
    }

	voice->SetVolume(Mathf::Clamp(volume, 0.0f, 1.0f));

    XAUDIO2_BUFFER buffer = {};
    buffer.AudioBytes = static_cast<UINT32>(clip->GetAudioData().size());
    buffer.pAudioData = clip->GetAudioData().data();
    buffer.Flags = XAUDIO2_END_OF_STREAM;

	HRESULT hr = voice->SubmitSourceBuffer(&buffer);
    if (FAILED(hr))
    {
        OutputDebugStringA("AudioManager Error: Failed to submit audio buffer for one-shot playback.\n");
        voice->DestroyVoice();
        return;
	}

    voice->Start(0);
	m_oneShotVoices.push_back({ voice, false });
}

void AudioManager::CleanupFinishedVoices()
{
    for (auto& os : m_oneShotVoices)
    {
        if (!os.finished)
        {
            XAUDIO2_VOICE_STATE state;
            os.voice->GetState(&state);
            if (state.BuffersQueued == 0)
            {
                os.voice->Stop(0);
                os.voice->DestroyVoice();
                os.voice = nullptr;
                os.finished = true;
            }
        }
    }
    m_oneShotVoices.erase(
        std::remove_if(m_oneShotVoices.begin(), m_oneShotVoices.end(),
                       [](const OneShotVoice& os) { return os.finished; }),
		m_oneShotVoices.end()
    );
}
