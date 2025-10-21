#pragma once

class ProjectileSystem : public System
{
private:
	void Update(ComponentManager& cm, World& world) override;
};