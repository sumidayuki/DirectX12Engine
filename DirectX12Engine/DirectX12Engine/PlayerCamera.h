#pragma once

struct PlayerCamera : IComponentData
{
	Entity player;
	Vector3 offset = Vector3::zero;
	float sensitivity = 10;

	float targetScale = 1.0f;
	float currentScale = 1.0f;
	float zoomSpeed = 5.0f;

	float moveLerpFactor = 10.0f;
	float rotLerpFactor = 10.0f;
	float zoomLerpFactor = 5.0f;
};