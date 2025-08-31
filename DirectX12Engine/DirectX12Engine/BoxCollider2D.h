#pragma once

/// <summary>
/// 矩形コライダーを表します。
/// コライダーの大きさとコライダーの状態を持ちます。
/// </summary>
struct BoxCollider2D
{
	Vector2 rect;
	CollisionInfo info;
};