#include "AudioAPI.h"

namespace AudioAPI
{
	void Play(AudioSource* source)
	{
		if (source && source->clip)
			source->requestPlay = true;
	}

	void Stop(AudioSource* source)
	{
		if (source)
			source->requestStop = true;
	}

	void SetClip(AudioSource* source, AudioClip* clip)
	{
		if (!source) return;
		if (source->isPlaying)
			source->requestStop = true;
		source->clip = clip;
	}

	void PlayOneShot(AudioSource* source, AudioClip* clip, float volumeScale)
	{
		if (!clip) return;
		float vol = volumeScale;
		if (source) vol *= source->volume;
		AudioManager::GetInstance()->PlayOneShotInternal(clip, vol);
	}

	void PlayClipAtPoint(World& world, AudioClip* clip, const Vector3& position, float volume)
	{
		if (!clip) return;

		Entity e = world.CreateEntity("OneShotAudio");
		Transform* t = world.GetComponent<Transform>(e);
		if (t) t->position = position;

		AudioSource src;
		src.clip = clip;
		src.volume = volume;
		src.spatialBlend = 1.0f;
		src.loop = false;
		src.playOnAwake = true;
		world.AddComponent<AudioSource>(e, src);
	}
}
