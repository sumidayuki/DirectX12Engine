#include "Precompiled.h"
#include "BoneSocketSystem.h"
#include "BoneSocket.h"

void BoneSocketSystem::Start(World& world)
{
}

void BoneSocketSystem::Update(World& world)
{
	View<BoneSocket, Transform> view(world);

	for (auto [entity, socket, transform] : view)
	{
		if (socket.targetEntity == INVALID_ENTITY)
		{
			continue;
		}

		// Animator
		Animator* targetAnimator = world.GetComponent<Animator>(socket.targetEntity);
		if (!targetAnimator || !targetAnimator->skeleton)
		{
			continue;
		}

		// Target Transform
		Transform* targetTransform = world.GetComponent<Transform>(socket.targetEntity);
		if (!targetTransform)
		{
			continue;
		}

		// Bone検索
		const auto& boneInfoMap = targetAnimator->skeleton->GetBoneInfoMap();

		auto it = boneInfoMap.find(socket.targetBoneName);
		if (it == boneInfoMap.end())
		{
			continue;
		}

		const int boneID = it->second.id;
		if (boneID < 0 || boneID >= targetAnimator->socketGlobalMatrices.size())
		{
			continue;
		}

		// BoneのWorld Matrixを作成
		const Matrix4x4& boneGlobalTransform = targetAnimator->socketGlobalMatrices[boneID];

		const Matrix4x4& targetWorldMatrix = targetTransform->localToWorldMatrix;

		Matrix4x4 finalWorldMatrix = boneGlobalTransform * targetWorldMatrix;

		// Bone World Matrixを分解
		Vector3 boneScale;
		Vector3 bonePosition;
		Quaternion boneRotation;

		finalWorldMatrix.Decompose(boneScale, boneRotation, bonePosition);

		// Offsetの基準となるRotation
		Quaternion baseRotation;

		switch (socket.offsetSpace)
		{
		case BoneSocketSpace::Bone:
			// ボーンの向きを基準にする
			baseRotation = boneRotation;

			break;

		case BoneSocketSpace::Target:
			// キャラクターなどtargetEntityの
			// 向きを基準にする
			baseRotation = targetTransform->rotation;

			break;

		default:
			baseRotation = boneRotation;

			break;
		}

		// Position
		Vector3 worldOffset = baseRotation * socket.posOffset;
		transform.position = bonePosition + worldOffset;

		// Rotation
		transform.rotation = baseRotation * socket.rotOffset;

		// Scale
		if (socket.followScale)
		{
			transform.scale = boneScale * socket.scaleOffset;
		}

		transform.dirty = true;
		transform.hasChanged = true;
	}
}