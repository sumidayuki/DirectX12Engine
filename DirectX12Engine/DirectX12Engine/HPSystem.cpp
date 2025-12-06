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

		if (damageable.damageQueue.empty())
		{
			continue;
		}

		for (int i = 0; i < damageable.damageQueue.size(); i++)
		{
			Damage damage = damageable.damageQueue.front();
			switch (damage.type)
			{
			case DamageType::Normal:
				hp.currentHP -= damage.damage;
				damageable.damageQueue.pop();
				break;

			default:
				break;
			}
		}
	}
}
