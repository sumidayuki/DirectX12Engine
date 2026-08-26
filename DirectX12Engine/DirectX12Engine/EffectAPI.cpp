#include "EffectAPI.h"

namespace EffectAPI
{
	void Play(EffectSource* source)
	{
		if (source && source->effect)
		{
			source->requestPlay = true;
		}
	}

	void Stop(EffectSource* source)
	{
		if (source)
		{
			source->requestStop = true;
		}
	}

	void SetEffect(EffectSource* source, Effect* effect)
	{
		if (!source)
		{
			return;
		}

		if (source->isPlaying)
		{
			source->requestStop = true;
		}

		source->effect = effect;
	}

	void PlayEffectAtPoint(World& world, Effect* effect, const Vector3& pos, float lifeTime, const Color& color, float scaleFactor)
	{
		if (!effect)
		{
			return;
		}

		Entity e = world.CreateEntity("OneShotEffect");

		Transform* transform = world.GetComponent<Transform>(e);
		transform->position = pos;
		transform->scale = Vector3(1.0f * scaleFactor, 1.0f * scaleFactor, 1.0f * scaleFactor);

		EffectSource source;
		source.effect = effect;
		source.loop = false;
		source.lifeTime = lifeTime;
		source.followTransform = true;
		source.playOnAwake = true;
		source.color = color;

		world.AddComponent<EffectSource>(e, source);

		world.AddOneShotEntity(effect->GetName(), e);
	}

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
		const Color& color)
	{
		if (!effect)
		{
			return;
		}

		Entity effectEntity = world.CreateEntity("OneShotEffect");

		Transform* transform = world.GetComponent<Transform>(effectEntity);
		transform->position = Vector3::zero;
		transform->rotation = Quaternion::identity;
		transform->scale = Vector3::one;
		transform->dirty = true;

		BoneSocket socket;
		socket.targetEntity = target;
		socket.targetBoneName = boneName;
		socket.offsetSpace = space;
		socket.posOffset = posOffset;
		socket.rotOffset = rotOffset;
		socket.scaleOffset = Vector3(scaleFactor, scaleFactor, scaleFactor);
		socket.followScale = true;
		world.AddComponent<BoneSocket>(effectEntity, socket);

		EffectSource source;
		source.effect = effect;
		source.loop = false;
		source.lifeTime = lifeTime;
		source.followTransform = true;
		source.playOnAwake = true;
		source.color = color;
		world.AddComponent<EffectSource>(effectEntity, source);

		world.AddOneShotEntity(effect->GetName(), effectEntity);
	}
}