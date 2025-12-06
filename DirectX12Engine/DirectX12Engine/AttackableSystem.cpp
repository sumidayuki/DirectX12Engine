#include "AttackableSystem.h"

void AttackableSystem::Update(World& world)
{
	View<Attackable> view(world);

	for (auto [entity, attackable] : view)
	{
		if (attackable.entities.empty())
		{
			continue;
		}

		if (!attackable.isAttacking)
		{
			attackable.entities.clear();
		}
	}
}