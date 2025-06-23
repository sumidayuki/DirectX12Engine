#pragma once

class EnemySpawnerSystem : public System
{
private:
	float m_elapsedTime;
	std::vector<EnemySpawnInfo> spawnList;

public:
	void Update(ComponentManager& cm, World& world) override;
};