#pragma once

struct Projectile
{
	Vector3 velocity = Vector3::zero;
	float speed = 10.0f;
	float lifeTime = 5.0f;
};