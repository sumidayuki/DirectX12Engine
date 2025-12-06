#pragma once

struct SphereCollider : IComponentData
{
	bool isEnable = true;
	float radius;
	Vector3 offset = Vector3::zero;
	CollisionInfo info;
};