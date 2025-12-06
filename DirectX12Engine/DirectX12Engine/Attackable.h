#pragma once

struct Attackable : IComponentData
{
	bool isAttacking = false;
	std::vector<Entity> entities;
};