#pragma once

enum class BulletType
{
	normal,
	laser
};

struct BossBullet
{
	int damage;
	BulletType bulletType;
};