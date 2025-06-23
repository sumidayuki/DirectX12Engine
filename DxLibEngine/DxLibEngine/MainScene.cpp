#include "MainScene.h"
#include "SystemList.h"

void MainScene::Start()
{
	// Worldにシステムを追加
	m_world.AddSystem(std::make_unique<InputSystem>());
	m_world.AddSystem(std::make_unique<RendererSystem>());
	m_world.AddSystem(std::make_unique<CollisionSystem>());
	m_world.AddSystem(std::make_unique<PlayerSystem>());
	m_world.AddSystem(std::make_unique<BulletSystem>());
	m_world.AddSystem(std::make_unique<EnemySystem>());
	m_world.AddSystem(std::make_unique<EnemySpawnerSystem>());
	m_world.AddSystem(std::make_unique<EnemyVSystem>());
	m_world.AddSystem(std::make_unique<EnemyStandardSystem>());
	m_world.AddSystem(std::make_unique<EnemyCircleSystem>());
	m_world.AddSystem(std::make_unique<EnemyWaveSystem>());
	m_world.AddSystem(std::make_unique<BossSystem>());
	m_world.AddSystem(std::make_unique<BossBulletSystem>());
	m_world.AddSystem(std::make_unique<TransformSystem>());

	// 背景の作成
	Entity background = m_world.CreateEntity();
	{
		Transform* bcTransform = m_world.GetComponent<Transform>(background);
		bcTransform->position = Vector3(Screen::GetWidth() / 2, Screen::GetHeight() / 2, 1000);
		m_world.AddComponent<RenderCommand>(background, RenderCommand{ .layer = Layer::BackGround, .type = RenderType::Sprite, .sprite = Sprite{.handle = LoadGraph("Assets/bc-01.png"), .width = 1920, .height = 1080 } });
	}

	// プレイヤーの作成
	Entity player = m_world.CreateEntity();
	{
		m_world.AddComponent<Player>(player, Player{});
		Transform* playerTransform = m_world.GetComponent<Transform>(player);
		playerTransform->position = Vector3(Screen::GetWidth() / 2, 720, 50);
		playerTransform->scale = Vector3(0.15f, 0.15f, 0);
		m_world.AddComponent<Input>(player, Input{});
		m_world.AddComponent<Velocity>(player, Velocity{ .speed = 350 });
		m_world.AddComponent<Status>(player, Status{ 3 });
		m_world.AddComponent<CircleCollider2D>(player, CircleCollider2D{ .radius = 40 });
		int w, h;
		GetGraphSize(LoadGraph("Assets/player_01.png"), &w, &h);
		m_world.AddComponent<RenderCommand>(player, RenderCommand{ .layer = Layer::Player, .type = RenderType::Sprite, .sprite = Sprite{.handle = LoadGraph("Assets/player_01.png"), .width = w, .height = h, .pivot = 0.5f } });
	}

	// 通常の敵を事前に読み込んでおく
	Sprite normalEnemy;
	{
		int normalEnemyHandle = LoadGraph("Assets/enemy_01.png");

		int w, h;
		GetGraphSize(normalEnemyHandle, &w, &h);

		normalEnemy = Sprite{ .handle = normalEnemyHandle, .width = w, .height = h };
	}

	// 円運動行う敵も事前に読み込んでおく
	Sprite circleEnemy;
	{
		int circleEnemyHandle = LoadGraph("Assets/enemy_02.png");

		int w, h;
		GetGraphSize(circleEnemyHandle, &w, &h);

		circleEnemy = Sprite{ .handle = circleEnemyHandle, .width = w, .height = h };
	}

	// ボスようの画像ファイルパスを事前に読み込んでおく
	Sprite boss;
	{
		int bossHandle = LoadGraph("Assets/boss-01.png");
		int w, h;
		GetGraphSize(bossHandle, &w, &h);

		boss = Sprite{ .handle = bossHandle, .width = w, .height = h };
	}

	// エネミーの生成手順
	// まずメインシーンなどで EnemySpawnMap（EnemySpawnInfoを格納する）を生成します。
	// EnemySpawnerSystem で EnemySpawnMap の中の EnemySpawnInfo の情報を基に隊列を生成していきます。
	// その後、それぞれの隊列が生成された EnemySpawnInfo の中から自分の隊列のものがあれば EnemySpawnInfo
	// の情報を基にEnemyを生成します。
	// エネミー生成用のスポナーを作成
	Entity enemySpawner = m_world.CreateEntity();
	Transform* spawnerTransform = m_world.GetComponent<Transform>(enemySpawner);
	spawnerTransform->position = Vector3(0, 0, 100);
	m_world.AddComponent<EnemySpawnMap>
		(
			enemySpawner,	// コンポーネントを追加するエンティティを指定する 
			EnemySpawnMap	// エネミーの生成マップ
			{
				.map =
				{
					EnemySpawnInfo
					{
						.spawnTime = 0.0f,
						.coolDown = 0.5f,
						.times = 5,
						.position = Vector3(960, 0, 50),
						.formation = Formation::Standard,
						.enemySprite = normalEnemy
					},
					EnemySpawnInfo
					{
						.spawnTime = 7.0f,
						.coolDown = 0.2f,
						.times = 7,
						.position = Vector3(700, 0, 50),
						.formation = Formation::V,
						.enemySprite = normalEnemy
					},
					EnemySpawnInfo
					{
						.spawnTime = 20.0f,
						.coolDown = 0.5f,
						.times = 7,
						.position = Vector3(1400, 0, 50),
						.formation = Formation::Standard,
						.enemySprite = normalEnemy
					},
					EnemySpawnInfo
					{
						.spawnTime = 24.0f,
						.coolDown = 0.5f,
						.times = 7,
						.position = Vector3(1000, 0, 50),
						.formation = Formation::Standard,
						.enemySprite = normalEnemy
					},
					EnemySpawnInfo
					{
						.spawnTime = 28.0f,
						.coolDown = 0.5f,
						.times = 7,
						.position = Vector3(600, 0, 50),
						.formation = Formation::Standard,
						.enemySprite = normalEnemy
					},
					EnemySpawnInfo
					{
						.spawnTime = 32,
						.coolDown = 0.5f,
						.times = 7,
						.position = Vector3(0, -400, 50),
						.formation = Formation::Circle,
						.enemySprite = circleEnemy
					},
					EnemySpawnInfo
					{
						.spawnTime = 36,
						.coolDown = 0.5f,
						.times = 6,
						.position = Vector3(600, 0, 50),
						.formation = Formation::Wave,
						.enemySprite = normalEnemy
					},
					EnemySpawnInfo
					{
						.spawnTime = 40,
						.coolDown = 0.5f,
						.times = 6,
						.position = Vector3(1500, 0, 50),
						.formation = Formation::Wave,
						.enemySprite = normalEnemy,
						.right = false
					},
					EnemySpawnInfo
					{
						.spawnTime = 44,
						.coolDown = 0.5f,
						.times = 6,
						.position = Vector3(600, 0, 50),
						.formation = Formation::Wave,
						.enemySprite = normalEnemy
					},
					EnemySpawnInfo
					{
						.spawnTime = 48,
						.coolDown = 0.5f,
						.times = 6,
						.position = Vector3(1500, 0, 50),
						.formation = Formation::Wave,
						.enemySprite = normalEnemy,
						.right = false
					},
					EnemySpawnInfo
					{
						.spawnTime = 52.0f,
						.coolDown = 0.2f,
						.times = 5,
						.position = Vector3(1000, 0, 50),
						.formation = Formation::V,
						.enemySprite = normalEnemy
					},
					EnemySpawnInfo
					{
						.spawnTime = 54.0f,
						.coolDown = 0.2f,
						.times = 5,
						.position = Vector3(500, 0, 50),
						.formation = Formation::V,
						.enemySprite = normalEnemy
					},
					EnemySpawnInfo
					{
						.spawnTime = 56.0f,
						.coolDown = 0.5f,
						.times = 7,
						.position = Vector3(1400, 0, 50),
						.formation = Formation::Standard,
						.enemySprite = normalEnemy
					},
					EnemySpawnInfo
					{
						.spawnTime = 60,
						.coolDown = 0,
						.times = 1,
						.position = Vector3(Screen::GetWidth() / 2, -300, 50),
						.formation = Formation::Boss,
						.enemySprite = boss,
					}
				}
			}
		);

		m_world.Start(m_world);
}

void MainScene::Update()
{
	m_world.Update(m_world);
}

void MainScene::Draw()
{
	m_world.Draw(m_world);
}
