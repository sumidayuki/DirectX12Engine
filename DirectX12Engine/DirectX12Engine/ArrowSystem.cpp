#include "ArrowSystem.h"
#include "Arrow.h"

void ArrowSystem::Update(World& world)
{
	View<Arrow, Collider, Attackable> view(world);

	for (auto [entity, arrow, coll, attackable] : view)
	{
		if (coll.info.state == CollisionState::Enter || coll.info.state == CollisionState::Stay)
		{
			attackable.isAttacking = true;
			for (int i = 0; i < attackable.entities.size(); i++)
			{
				if (attackable.entities[i] == coll.info.other)
				{
					return;
				}
			}

			Damageable* damageable = world.GetComponent<Damageable>(coll.info.other);
			if (damageable)
			{
				Damage damage;
				damage.type = DamageType::Normal;
				damage.damage = 10;
				damageable->damageQueue.push(damage);
				attackable.entities.push_back(coll.info.other);
				world.DestroyEntity(entity);
			}
		}
	}
}
