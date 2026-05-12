#pragma once

struct PlayerCamera : IComponentData
{
	Entity player;
	Vector3 offset = Vector3::zero;
	float sensitivity = 10;
};