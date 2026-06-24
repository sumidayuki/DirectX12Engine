#pragma once

namespace AudioAPI
{
	void Play(AudioSource* source);
	void Stop(AudioSource* source);
	void SetClip(AudioSource* source, AudioClip* clip);
	void PlayOneShot(AudioSource* source, AudioClip* clip, float volumeScale = 1.0f);
	void PlayClipAtPoint(World& world, AudioClip* clip, const Vector3& position, float volume = 1.0f);
}
