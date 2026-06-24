#include "StaminaSystem.h"

void StaminaSystem::Update(World& world)
{
	View<Stamina, Input> view(world);

	for(auto [entity, stamina, input] : view)
	{
		Entity staminaBarEntity = UIManager::GetInstance()->GetUIObject(HashString("MainSceneUI"), HashString(stamina.barName));
		Slider* staminaBar = world.GetComponent<Slider>(staminaBarEntity);

		// スタミナが最大値未満のとき、スタミナバーを減少させる
		if(stamina.value < stamina.maxValue)
		{
			// スタミナバーの値がスタミナの値より大きい場合、スタミナバーをスタミナの値に近づける
			if(stamina.value / stamina.maxValue <= staminaBar->value)
			{
				staminaBar->value = Mathf::Lerp(staminaBar->value, stamina.value / stamina.maxValue, Time::GetDeltaTime() * 5);
			}

			// スタミナが減少してからの経過時間が回復開始までの遅延時間未満の場合、経過時間を増加させる
			if(stamina.timer <= stamina.regenDelay)
			{
				stamina.timer += Time::GetDeltaTime();
			}
			// スタミナが減少してからの経過時間が回復開始までの遅延時間以上の場合、スタミナを回復させる
			else
			{
				stamina.value += stamina.regenRate * Time::GetDeltaTime();

				staminaBar->value = stamina.value / stamina.maxValue;

				if(stamina.value > stamina.maxValue)
				{
					stamina.value = stamina.maxValue;
				}
			}
		}
	}
}
