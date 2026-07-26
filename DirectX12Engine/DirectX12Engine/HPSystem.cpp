#include "Precompiled.h"
#include "HPSystem.h"

void HPSystem::Update(World& world)
{
	View<HP, Damageable, Animator> view(world);

	for (auto [entity, hp, damageable, animator] : view)
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

		// 被ダメージ処理（Hitアニメーション再生）
		if (!damageable.damageQueue.empty())
		{
			Damage damage = damageable.damageQueue.front();
			if (damage.type == DamageType::Normal)
			{
				if (!hp.isInvincible)
				{
					animator.isLoop = false;
					AnimationSystem::Play(animator, "Hit_00", true);
					hp.currentHP -= damage.damage;
					hp.hpBar->value = hp.currentHP;
				}

				damageable.damageQueue.pop();
			}
		}
	}
}
