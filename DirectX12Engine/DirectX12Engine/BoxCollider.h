#pragma once

struct BoxCollider : IComponentData
{
	bool isEnable = true;
    bool isTrigger = false;
	Vector3 size;
	Vector3 offset = Vector3::zero;
	CollisionInfo info;
};