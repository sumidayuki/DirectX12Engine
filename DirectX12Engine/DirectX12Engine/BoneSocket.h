#pragma once

struct BoneSocket : IComponentData
{
	// 追従対象のAnimatorを持ったEntity
	Entity targetEntity = INVALID_ENTITY;

	// 追従したいボーンの名前
	std::string targetBoneName;

	// オフセット
	Vector3 posOffset = Vector3::zero;
	Quaternion rotOffset = Quaternion::identity;
};