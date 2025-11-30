#pragma once

struct PlayerCamera : IComponentData
{
	Entity player;
	Vector3 offset;
	float sensitivity;
};