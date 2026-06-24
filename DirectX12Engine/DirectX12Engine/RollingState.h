#pragma once

struct RollingState : IComponentData
{
	Vector3 direction;		// 移動方向
	float speed;			// 移動速度
	float duration;			// 移動時間
	float timer;			// 経過時間
	float invincibleStart;	// 無敵時間の開始時間
	float invincibleEnd;	// 無敵時間の終了時間
	bool isInvincible = false;		// 無敵状態かどうか
	bool isRolling = false;			// ローリング状態かどうか
};