#pragma once

struct Attackable : IComponentData
{
	bool isAttacking = false;
	float damage = 0.0f;
	DamageType damageType = DamageType::Normal;
	std::vector<Entity> entities;
};