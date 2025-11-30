#pragma once

class GameManagerSystem : public System
{
private:
	bool Load(World& world) override;
	void Start(World& world) override;
	void Update(World& world) override;
};