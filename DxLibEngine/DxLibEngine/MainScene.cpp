#include "MainScene.h"
#include "SystemList.h"

void MainScene::Start()
{
	// Worldにシステムを追加
	m_world.AddSystem(std::make_unique<CameraSystem>());
	m_world.AddSystem(std::make_unique<SpriteRendererSystem>());
	m_world.AddSystem(std::make_unique<MeshRendererSystem>());
	m_world.AddSystem(std::make_unique<TransformSystem>());
	m_world.AddSystem(std::make_unique<InputSystem>());
	m_world.AddSystem(std::make_unique<CollisionSystem>());
	m_world.AddSystem(std::make_unique<PlayerSystem>());
	m_world.AddSystem(std::make_unique<BulletSystem>());
	m_world.AddSystem(std::make_unique<EnemySystem>());
	m_world.AddSystem(std::make_unique<EnemySpawnerSystem>());
	m_world.AddSystem(std::make_unique<EnemyVSystem>());
	m_world.AddSystem(std::make_unique<EnemyStandardSystem>());
	m_world.AddSystem(std::make_unique<EnemyCircleSystem>());
	m_world.AddSystem(std::make_unique<EnemyWaveSystem>());
	//m_world.AddSystem(std::make_unique<BossSystem>());
	//m_world.AddSystem(std::make_unique<BossBulletSystem>());

	//Entity* camera = m_world.CreateCamera2D(1920, 1080, Vector3(1920 / 2, 1080 / 2, -10));
	//
	//Entity* bg = m_world.CreateWithSprite(L"Assets/bc-01.png", Rect(0, 0, 1920, 1080), Vector2::zero, 1.0f, nullptr, Vector3(0, 0, 100));
	//
	//TextureImporter importer;
	//
	//// 通常の敵を事前に読み込んでおく
	//Texture2D* normalEnemy = importer.Import(L"Assets/enemy_01.png");
	//
	//// 円運動行う敵も事前に読み込んでおく
	//Texture2D* circleEnemy = importer.Import(L"Assets/enemy_02.png");
	//
	//// ボスようの画像ファイルパスを事前に読み込んでおく
	//Texture2D* boss = importer.Import(L"Assets/boss-01.png");
	//
	//// エネミーの生成手順
	//// まずメインシーンなどで EnemySpawnMap（EnemySpawnInfoを格納する）を生成します。
	//// EnemySpawnerSystem で EnemySpawnMap の中の EnemySpawnInfo の情報を基に隊列を生成していきます。
	//// その後、それぞれの隊列が生成された EnemySpawnInfo の中から自分の隊列のものがあれば EnemySpawnInfo
	//// の情報を基にEnemyを生成します。
	//// エネミー生成用のスポナーを作成
	//Entity* enemySpawner = m_world.CreateEntity();
	//Transform* spawnerTransform = m_world.GetComponent<Transform>(*enemySpawner);
	//spawnerTransform->position = Vector3(0, 0, 100);
	//m_world.AddComponent<EnemySpawnMap>
	//	(
	//		*enemySpawner,	// コンポーネントを追加するエンティティを指定する 
	//		EnemySpawnMap	// エネミーの生成マップ
	//		{
	//			.map =
	//			{
	//				EnemySpawnInfo
	//				{
	//					.spawnTime = 0.0f,
	//					.coolDown = 0.5f,
	//					.times = 5,
	//					.position = Vector3(960, 1080, 50),
	//					.formation = Formation::Standard,
	//					.texture = normalEnemy
	//				},
	//				EnemySpawnInfo
	//				{
	//					.spawnTime = 7.0f,
	//					.coolDown = 0.2f,
	//					.times = 7,
	//					.position = Vector3(700, 1080, 50),
	//					.formation = Formation::V,
	//					.texture = normalEnemy
	//				},
	//				EnemySpawnInfo
	//				{
	//					.spawnTime = 20.0f,
	//					.coolDown = 0.5f,
	//					.times = 7,
	//					.position = Vector3(1400, 1080, 50),
	//					.formation = Formation::Standard,
	//					.texture = normalEnemy
	//				},
	//				EnemySpawnInfo
	//				{
	//					.spawnTime = 24.0f,
	//					.coolDown = 0.5f,
	//					.times = 7,
	//					.position = Vector3(1000, 1080, 50),
	//					.formation = Formation::Standard,
	//					.texture = normalEnemy
	//				},
	//				EnemySpawnInfo
	//				{
	//					.spawnTime = 28.0f,
	//					.coolDown = 0.5f,
	//					.times = 7,
	//					.position = Vector3(600, 1080, 50),
	//					.formation = Formation::Standard,
	//					.texture = normalEnemy
	//				},
	//				EnemySpawnInfo
	//				{
	//					.spawnTime = 32,
	//					.coolDown = 0.5f,
	//					.times = 7,
	//					.position = Vector3(0, 1480, 50),
	//					.formation = Formation::Circle,
	//					.texture = circleEnemy
	//				},
	//				EnemySpawnInfo
	//				{
	//					.spawnTime = 36,
	//					.coolDown = 0.5f,
	//					.times = 6,
	//					.position = Vector3(600, 1080, 50),
	//					.formation = Formation::Wave,
	//					.texture = normalEnemy
	//				},
	//				EnemySpawnInfo
	//				{
	//					.spawnTime = 40,
	//					.coolDown = 0.5f,
	//					.times = 6,
	//					.position = Vector3(1500, 1080, 50),
	//					.formation = Formation::Wave,
	//					.texture = normalEnemy,
	//					.right = false
	//				},
	//				EnemySpawnInfo
	//				{
	//					.spawnTime = 44,
	//					.coolDown = 0.5f,
	//					.times = 6,
	//					.position = Vector3(600, 1080, 50),
	//					.formation = Formation::Wave,
	//					.texture = normalEnemy
	//				},
	//				EnemySpawnInfo
	//				{
	//					.spawnTime = 48,
	//					.coolDown = 0.5f,
	//					.times = 6,
	//					.position = Vector3(1500, 1080, 50),
	//					.formation = Formation::Wave,
	//					.texture = normalEnemy,
	//					.right = false
	//				},
	//				EnemySpawnInfo
	//				{
	//					.spawnTime = 52.0f,
	//					.coolDown = 0.2f,
	//					.times = 5,
	//					.position = Vector3(1000, 1080, 50),
	//					.formation = Formation::V,
	//					.texture = normalEnemy
	//				},
	//				EnemySpawnInfo
	//				{
	//					.spawnTime = 54.0f,
	//					.coolDown = 0.2f,
	//					.times = 5,
	//					.position = Vector3(500, 1080, 50),
	//					.formation = Formation::V,
	//					.texture = normalEnemy
	//				},
	//				EnemySpawnInfo
	//				{
	//					.spawnTime = 56.0f,
	//					.coolDown = 0.5f,
	//					.times = 7,
	//					.position = Vector3(1400, 1080, 50),
	//					.formation = Formation::Standard,
	//					.texture = normalEnemy
	//				},
	//				EnemySpawnInfo
	//				{
	//					.spawnTime = 60,
	//					.coolDown = 0,
	//					.times = 1,
	//					.position = Vector3(Screen::GetWidth() / 2, 1380, 50),
	//					.formation = Formation::Boss,
	//					.texture = normalEnemy
	//				}
	//			}
	//		}
	//	);

	// --- 3Dカメラの作成 ---
	float fov = 60.0f;
	float aspect = (float)Screen::GetWidth() / (float)Screen::GetHeight();
	float nearPlane = 0.1f; // 非常に近いオブジェクトも描画できるように調整
	float farPlane = 1000.0f;
	Vector3 cameraPos = Vector3(0.0f, 1.5f, -5.0f); // 少し上から、少し手前に引いた位置
	
	Entity* camera = m_world.CreateCamera3D(fov, aspect, nearPlane, farPlane, cameraPos);

	// 1. TextureImporterを使ってテクスチャをロードする
	TextureImporter texImporter;
	ComPtr<Texture2D> rockTexture;
	rockTexture.Attach(texImporter.Import(L"Assets/player_01.png"));

	// 2. ModelImporterを使ってFBXモデルをロードする
	ModelImporter modelImporter;
	// Assimpはマルチバイト文字のパスを扱うので、stringに変換します
	std::string path = "Assets/Knight.fbx";
	auto loadedMeshes = modelImporter.Import(path);

	// 3. 読み込みが成功したかチェック
	if (!loadedMeshes.empty() && rockTexture)
	{
		// 4. マテリアルを作成し、ロードしたテクスチャを設定
		ComPtr<Material> rockMaterial;
		rockMaterial.Attach(new Material());
		rockMaterial->SetMainTexture(rockTexture.Get());
		// テクスチャの色をそのまま使いたいので、マテリアルの色は白のまま
		rockMaterial->SetColor(Color::white);

		// 5. エンティティを作成し、コンポーネントをアタッチ
		Entity* rockEntity = m_world.CreateEntity();

		// Transformコンポーネント
		Transform* rockTransform = m_world.GetComponent<Transform>(*rockEntity);
		rockTransform->position = Vector3(0.0f, 0.0f, 0.0f);
		// モデルが大きすぎる/小さすぎる場合があるので、スケールで調整
		rockTransform->scale = Vector3(0.01f, 0.01f, 0.01f);

		// MeshRendererコンポーネント
		m_world.AddComponent<MeshRenderer>(*rockEntity, MeshRenderer{});
		MeshRenderer* rockRenderer = m_world.GetComponent<MeshRenderer>(*rockEntity);
		// 読み込んだメッシュ（モデルに複数の部品がある場合もあるが、今回は最初の1つ）
		rockRenderer->mesh = loadedMeshes[0];
		// 作成したマテリアル
		rockRenderer->material = rockMaterial;
	}

	m_world.Start(m_world);
}

void MainScene::Update()
{
	View<Transform, MeshRenderer> view(m_world.GetComponentManager());
	for (auto [entity, transform, renderer] : view)
	{
		m_world.GetSystem<TransformSystem>()->Rotate(transform, Vector3(0, 1, 0), 30.0f * Time::GetDeltaTime());
	}


	m_world.Update(m_world);
}

void MainScene::Draw()
{
	m_world.Draw(m_world);
}
