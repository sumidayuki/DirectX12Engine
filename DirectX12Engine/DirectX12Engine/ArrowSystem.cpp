#include "ArrowSystem.h"
#include "Arrow.h"

void ArrowSystem::Update(World& world)
{
	View<Arrow, Collider, Attackable> view(world);

	for (auto [entity, arrow, coll, attackable] : view)
	{
		if ((coll.info.state == CollisionState::Enter || coll.info.state == CollisionState::Stay))
		{
			if (world.GetComponent<Enemy>(coll.info.other))
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
					Effect* effect = AssetManager::GetInstance()->GetAsset<Effect>(AssetType::Effect, L"Assets/Effects/effect_arrow-attack_impact-00.efkefc");
					Transform* transform = world.GetComponent<Transform>(entity);
					Vector3 pos = TransformAPI::TransformPoint(*transform, Vector3(0, 0, -20));
					EffectAPI::PlayEffectAtPoint(world, effect, pos, 1, Color::white, 10 + attackable.damage);

					Damage damage;
					damage.type = attackable.damageType;
					damage.damage = attackable.damage;
					damageable->damageQueue.push(damage);
					attackable.entities.push_back(coll.info.other);
					world.DestroyEntity(entity);
				}
			}
		}
	}
}
