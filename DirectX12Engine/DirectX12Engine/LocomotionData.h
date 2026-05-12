#pragma once

enum class LocomotionState
{
	Idle,
	Moving,
	Turning
};

struct LocomotionData : IComponentData
{
	LocomotionState state = LocomotionState::Idle;

	Quaternion turnTargetRot;

	Vector3 currentVelocity = Vector3(0, 0, 0);
};