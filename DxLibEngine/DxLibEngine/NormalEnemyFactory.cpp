#include "NormalEnemyFactory.h"

void NormalEnemyFactory::Init(const int poolSize, World& world)
{
	// ’Êí‚Ì“G‚ğ–‘O‚É“Ç‚İ‚ñ‚Å‚¨‚­
	Sprite normalEnemy;
	{
		int normalEnemyHandle = LoadGraph("Assets/enemy_01.png");

		int w, h;
		GetGraphSize(normalEnemyHandle, &w, &h);

		normalEnemy = Sprite{ .handle = normalEnemyHandle, .width = w, .height = h };
	}

	// ‰~‰^“®s‚¤“G‚à–‘O‚É“Ç‚İ‚ñ‚Å‚¨‚­
	Sprite circleEnemy;
	{
		int circleEnemyHandle = LoadGraph("Assets/enemy_02.png");

		int w, h;
		GetGraphSize(circleEnemyHandle, &w, &h);

		circleEnemy = Sprite{ .handle = circleEnemyHandle, .width = w, .height = h };
	}

	for (int i = 0; i < poolSize; i++)
	{
		Entity enemy = world.CreateEntity();
		world.AddComponent<Enemy>(enemy, Enemy{ .formation = Formation::Standard });
		world.AddComponent<Sprite>(enemy, normalEnemy);
		world.AddComponent<CircleCollider2D>(enemy, CircleCollider2D{ .radius = 40 });
		world.AddComponent<Status>(enemy, Status{ .life = 1 });
	}
}

Entity NormalEnemyFactory::Spawn(World& world)
{
    return Entity();
}

void NormalEnemyFactory::Return(Entity e)
{
}
