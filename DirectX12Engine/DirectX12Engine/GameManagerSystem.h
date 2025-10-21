#pragma once

class GameManagerSystem : public System
{
private:
	bool Load(ComponentManager& cm, World& world) override;
	void Start(ComponentManager& cm, World& world) override;
	void Update(ComponentManager& cm, World& world) override;
};