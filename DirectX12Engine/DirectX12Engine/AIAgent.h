#pragma once

struct AIAgent
{
	Vector3 target = Vector3::max;
	float acceleration = 5.0f;
	float speed = 2.0f;
	float rotationSpeed = 45.0f;
	Vector3 velocity = Vector3::zero;
	bool updatePosition = true;
	bool updateRotation = true;
};