#pragma once

struct Enemy : IComponentData
{
	Entity target;
	Vector3 lastTargetPos = Vector3::zero;
	Vector3 startJumpPos = Vector3::zero;
	
	uint32_t currentState = 0;
	float stateTimer = 0.0f;
};