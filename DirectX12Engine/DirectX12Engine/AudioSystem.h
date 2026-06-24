#pragma once

class AudioSystem : public System
{
public:
	void Update(World& world) override;

private:
	void StartVoice(AudioSource& source);
};