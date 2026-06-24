#pragma once

class PlayerActionGuideSystem : public System
{
private:


private:
	void Start(World& world) override;
	void Update(World& world) override;

	void SetEnableAll(bool isEnable, World& world);
};