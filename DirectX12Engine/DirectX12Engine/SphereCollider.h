#pragma once

struct SphereCollider : IComponentData
{
	float radius;
	Vector3 offset = Vector3::zero;
	CollisionInfo info;
	std::string boneName = "";
};