#pragma once

class GuardSystem : public System
{
private:
	void Update(World& world) override;

private:
	Color GetGuardColor(float rete) const;
};