#pragma once

/// <summary>
/// 入力を受け取るシステムです。
/// </summary>
class InputSystem : public System
{
public:
	// Systemの更新処理のオーバーライドです。
	void Update(World& world) override;
};