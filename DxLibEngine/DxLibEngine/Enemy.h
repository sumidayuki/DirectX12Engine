#pragma once

/// <summary>
/// エネミータグです。自分がどの隊列に属しているのかを持ちます。
/// </summary>
struct Enemy
{
	Formation formation;

	float lifeTime = 0;
};