#pragma once

class NormalEnemyFactory : public IFactoryPattern
{
public:
	void Init(const int poolSize, World& world) override;
	Entity Spawn(World& world) override;
	void Return(Entity e) override;
};