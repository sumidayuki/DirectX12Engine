#pragma once

class GameManagerSystem : public System
{
private:
	Entity m_player;
	Entity m_enemy;

public:
	Entity GetPlayer() const { return m_player; }
	Entity GetEnemy() const { return m_enemy; }

private:
	bool Load(World& world) override;
	void Start(World& world) override;
	void Update(World& world) override;
};