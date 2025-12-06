#pragma once

struct AABBCollider : IComponentData
{
	bool isEnable = true;
	Bounds bounds;
	Vector3 offset = Vector3::zero;
	CollisionInfo info;
};