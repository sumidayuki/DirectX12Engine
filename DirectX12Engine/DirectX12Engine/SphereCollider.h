#pragma once

struct SphereCollider : IComponentData
{
	float radius;
	Vector3 offset = Vector3::zero;
	bool isEnable = true;
	bool isTrigger = false;
	CollisionInfo info;
};