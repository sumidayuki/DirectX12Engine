#pragma once

#include <Effekseer.h>
#include <EffekseerRendererDX12.h>

class EffectSystem : public System
{
private:
	void Update(World& world) override;
	void Draw(World& world) override;

private:
	void StartEffect(EffectSource& source, const Transform& transform);
	void StopEffect(EffectSource& source);
};
