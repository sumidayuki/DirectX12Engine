#include "Precompiled.h"
#include "HPSystem.h"

void HPSystem::Update(World& world)
{
	View<HP, Damageable> view(world);

	for (auto [entity, hp, damageable] : view)
	{
		if (hp.isDeath) continue;

		if (!hp.isInit)
		{
			hp.currentHP = hp.maxHP;
			hp.isInit = true;
		}

		if (hp.currentHP <= 0)
		{
			hp.isDeath = true;
		}
	}
}
