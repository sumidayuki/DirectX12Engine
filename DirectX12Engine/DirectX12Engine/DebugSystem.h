#pragma once

class DebugSystem : public System
{
private:
	bool m_isActive;

private:
	void Start(World& world) override;

	void Update(World& world) override;
};