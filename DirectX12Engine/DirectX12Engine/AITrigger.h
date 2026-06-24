#pragma once

struct AITrigger : IComponentData
{
	std::vector<uint32_t> triggers;
};