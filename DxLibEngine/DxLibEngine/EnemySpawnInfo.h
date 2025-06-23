#pragma once

/// <summary>
/// EnemySpawnInfo
/// ��ꐬ���FspawnTime�i�������ԁj		
/// ��񐬕��FcoolDown�i�����Ԋu�j
/// ��O�����Ftimes�i�������j
/// ��l�����Fposition�i������ʒu�j
/// ��ܐ����Fformation�i����p�^�[���j
/// ��Z�����FenemySprite�i�G�摜�j
/// </summary>
struct EnemySpawnInfo
{
	float spawnTime;		// �G���o�����鎞��
	float coolDown;
	int times;
	Vector3 position;		// �o������ʒu
	Formation formation;	// ����
	Sprite enemySprite;
	bool right = true;
	float timer = 0.0f;
};