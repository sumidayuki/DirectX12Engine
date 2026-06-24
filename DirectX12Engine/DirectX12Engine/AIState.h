#pragma once

struct AIState : IComponentData
{
	uint32_t currentStateID;
	uint32_t nextStateID;
	float stateTime;
};