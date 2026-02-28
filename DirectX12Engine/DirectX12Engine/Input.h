#pragma once

/// <summary>
/// Inputを扱うコンポーネントです。
/// </summary>
struct Input : IComponentData
{
	bool moveUp;		// 上方向
	bool moveDown;		// 下方向
	bool moveLeft;		// 左方向
	bool moveRight;		// 右方向
	bool shot;			// ショット
	bool dash;
	bool attack1;
	bool attack2;
	bool guard1;
	bool guard2;
	bool isGuard;

	float horizontal;
	float vartical;
	Vector2 direction;
};