#pragma once

enum DamageType
{
	Normal,
	Stan
};

struct Damage
{
	DamageType type;
	int damage;
};

struct Damageable : IComponentData
{
	std::queue<Damage> damageQueue;
};