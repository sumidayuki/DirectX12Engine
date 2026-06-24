#include "AudioSystem.h"

void AudioSystem::Update(World& world)
{
	Transform listenerTf;
	listenerTf.position = Vector3::zero;

	View<AudioListener, Transform> listenerView(world);
	for (auto [entity, listener, transform] : listenerView)
	{
		if (listener.enabled)
		{
			listenerTf = transform;
		}
	}

	View<AudioSource, Transform> view(world);
	for (auto [entity, source, transform] : view)
	{
		if (source.playOnAwake && source.clip && !source.isStarted)
		{
			source.isStarted = true;
			source.requestPlay = true;
		}

		if (source.requestStop)
		{
			source.requestStop = false;
			if (source.sourceVoice)
			{
				source.sourceVoice->Stop(0);
				source.sourceVoice->FlushSourceBuffers();
			}
			source.isPlaying = false;
		}

		if (source.requestPlay)
		{
			source.requestPlay = false;
			if (source.clip)
			{
				if (source.sourceVoice)
				{
					source.sourceVoice->Stop(0);
					source.sourceVoice->FlushSourceBuffers();
				}
				StartVoice(source);
			}
		}

		if (source.isPlaying && source.sourceVoice)
		{
			XAUDIO2_VOICE_STATE state;
			source.sourceVoice->GetState(&state);
			if (state.BuffersQueued == 0)
			{
				source.isPlaying = false;
			}
		}

		if (source.isPlaying && source.sourceVoice)
		{
			if (source.spatialBlend <= 0.0f)
			{
				source.sourceVoice->SetVolume(source.mute ? 0.0f : source.volume);
			}
			else
			{
				Vector3 srcPos = transform.position;
				if (transform.parent != INVALID_ENTITY)
				{
					srcPos = Vector3(
						transform.localToWorldMatrix.m[3][0],
						transform.localToWorldMatrix.m[3][1],
						transform.localToWorldMatrix.m[3][2]);
				}

				float dist = (listenerTf.position - srcPos).Magnitude();

				float atten = 1.0f;
				if (dist >= source.maxDistance)
					atten = 0.0f;
				else if (dist > source.minDistance)
					atten = 1.0f - (dist - source.minDistance) / (source.maxDistance - source.minDistance);

				float blended = Mathf::Lerp(1.0f, atten, source.spatialBlend);
				float vol = source.mute ? 0.0f : source.volume * blended;
				source.sourceVoice->SetVolume(std::clamp(vol, 0.0f, 1.0f));
			}

			source.sourceVoice->SetFrequencyRatio(source.pitch);
		}
	}

	std::vector<Entity> toDestroy;

	View<AudioSource> cleanupView(world);
	for (auto [entity, source] : cleanupView)
	{
		if (entity.name == "OneShotAudio" && !source.isPlaying && source.isStarted && !source.loop)
		{
			if (source.sourceVoice)
			{
				AudioManager::GetInstance()->DestroySourceVoice(source.sourceVoice);
				source.sourceVoice = nullptr;
			}
			toDestroy.push_back(entity);
		}
	}

	for (const Entity& e : toDestroy)
	{
		world.DestroyEntity(e);
	}

	AudioManager::GetInstance()->CleanupFinishedVoices();
}

void AudioSystem::StartVoice(AudioSource& src)
{
	if (!src.clip || src.clip->GetAudioData().empty()) return;

	AudioManager* mgr = AudioManager::GetInstance();
	if (!mgr) return;

	if (!src.sourceVoice)
	{
		src.sourceVoice = mgr->CreateSourceVoice(src.clip->GetFormat());
		if (!src.sourceVoice) return;
	}

	src.sourceVoice->SetVolume(src.mute ? 0.0f : src.volume);
	src.sourceVoice->SetFrequencyRatio(src.pitch);

	XAUDIO2_BUFFER buf = {};
	buf.AudioBytes = static_cast<UINT32>(src.clip->GetAudioData().size());
	buf.pAudioData = src.clip->GetAudioData().data();
	buf.Flags = XAUDIO2_END_OF_STREAM;
	if (src.loop) buf.LoopCount = XAUDIO2_LOOP_INFINITE;

	if (FAILED(src.sourceVoice->SubmitSourceBuffer(&buf))) return;

	src.sourceVoice->Start(0);
	src.isPlaying = true;
	src.isStarted = true;
}
