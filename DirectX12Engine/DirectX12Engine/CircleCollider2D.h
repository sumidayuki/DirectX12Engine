#pragma once

struct CircleCollider2D : IComponentData
{
	float radius;
	CollisionInfo info;
};