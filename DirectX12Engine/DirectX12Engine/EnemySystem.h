#pragma once

class EnemySystem : public System
{
private:
	void Start(ComponentManager& cm, World& world) override;

	void Update(ComponentManager& cm, World& world) override;
};