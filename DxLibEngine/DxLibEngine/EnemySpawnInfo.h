#pragma once

/// <summary>
/// EnemySpawnInfo
/// 第一成分：spawnTime（生成時間）		
/// 第二成分：coolDown（生成間隔）
/// 第三成分：times（生成数）
/// 第四成分：position（生成基準位置）
/// 第五成分：formation（隊列パターン）
/// 第六成分：enemySprite（敵画像）
/// </summary>
struct EnemySpawnInfo
{
	float spawnTime;		// 敵が出現する時間
	float coolDown;
	int times;
	Vector3 position;		// 出現する位置
	Formation formation;	// 隊列
	Texture2D* texture;
	bool right = true;
	float timer = 0.0f;
};