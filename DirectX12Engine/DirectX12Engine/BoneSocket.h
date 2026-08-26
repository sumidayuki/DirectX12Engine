#pragma once

enum class BoneSocketSpace
{
	// ボーンの向きを基準にする
	// 武器などに向いている
	Bone,

	// targetEntityの向きを基準にする
	// ガードエフェクトなどに向いている
	Target
};

struct BoneSocket : IComponentData
{
	// 追従対象のAnimatorを持ったEntity
	Entity targetEntity = INVALID_ENTITY;

	// 追従したいボーンの名前
	std::string targetBoneName;

	// オフセット
	Vector3 posOffset = Vector3::zero;
	Quaternion rotOffset = Quaternion::identity;
	Vector3 scaleOffset = Vector3::one;

	// Scaleをボーンから引き継ぐか
	bool followScale = false;

	// Position / Rotation のオフセット基準
	BoneSocketSpace offsetSpace = BoneSocketSpace::Bone;
};