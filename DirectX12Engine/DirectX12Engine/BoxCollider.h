#pragma once

struct BoxCollider : IComponentData
{
	Vector3 size;
	Vector3 offset = Vector3::zero;
	CollisionInfo info;
};