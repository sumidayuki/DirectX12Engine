#pragma once

class MovementSystem : public System
{
public:
	// Systemの更新処理のオーバーライドです。
	void Update(ComponentManager& cm, World& world) override;
};