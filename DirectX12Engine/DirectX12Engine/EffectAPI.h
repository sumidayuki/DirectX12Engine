#pragma once

namespace EffectAPI
{
	void Play(EffectSource* source);

	void Stop(EffectSource* source);

	void SetEffect(EffectSource* source, Effect* effect);

	void PlayEffectAtPoint(World& world, Effect* effect, const Vector3& pos, float lifeTime, const Color& color, float scaleFactor);

	void PlayEffectAttachedToBone(
		World& world,
		Effect* effect,
		Entity target,
		const std::string& boneName,
		float lifeTime,
		const BoneSocketSpace& space,
		const Vector3& posOffset,
		const Quaternion& rotOffset,
		float scaleFactor,
		const Color& color
	);
}