#pragma once

struct Damage
{
	DamageType type;
	int damage;
};

struct Damageable : IComponentData
{
	std::queue<Damage> damageQueue;
};