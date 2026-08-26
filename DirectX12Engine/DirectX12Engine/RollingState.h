#pragma once

struct RollingState : IComponentData
{
	// ローリング開始時に確定した移動方向
	Vector3 direction = Vector3(0, 0, 0);

	// ローリング初期化済みか
	bool isRolling = false;
};