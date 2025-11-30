#pragma once

struct AABBCollider : IComponentData
{
	Bounds bounds;
	Vector3 offset = Vector3::zero;
	CollisionInfo info;
};