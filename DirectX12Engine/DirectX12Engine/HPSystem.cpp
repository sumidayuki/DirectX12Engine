#include "Precompiled.h"
#include "HPSystem.h"
#include "PlayerTag.h"

void HPSystem::Update(World& world)
{
	View<HP, Damageable, Animator> view(world);

	for (auto [entity, hp, damageable, animator] : view)
	{
		if (hp.isDeath) continue;

		if (!hp.isInit)
		{
			bool isPlayer = world.HasComponent<PlayerTag>(entity);
			std::string name = isPlayer ? "Player" : "Enemy";

			hp.hpBar = world.GetComponent<Slider>(UIManager::GetInstance()->GetUIObject(HashString("MainSceneUI"), HashString((name + "_hp_bar").c_str())));
			hp.isSuperArmor = !isPlayer;
			hp.currentHP = hp.maxHP;
			hp.hpBar->minValue = 0;
			hp.hpBar->maxValue = hp.maxHP;
			hp.hpBar->value = hp.currentHP;
			hp.isInit = true;
		}

		if (hp.currentHP <= 0)
		{
			hp.isDeath = true;
		}

		if (!damageable.damageQueue.empty())
		{
			for (int i = 0; i < damageable.damageQueue.size(); i++)
			{
				if (!hp.isSuperInvincible)
				{
					Damage damage = damageable.damageQueue.front();
					if (!hp.isInvincible)
					{
						if (!hp.isSuperArmor)
						{
							animator.isLoop = false;
							AnimationSystem::Play(animator, "Hit_00", true);
						}
						hp.currentHP -= damage.damage;
						hp.hpBar->value = hp.currentHP;
					}

					damageable.damageQueue.pop();
				}
			}
		}
	}
}
