#include "MainScene.h"
#include "SystemList.h"
#include "DebugManager.h"
#include "ArrowSystem.h"
#include "CharacterImporter.h"

bool MainScene::Load()
{
	CharacterImporter::CreateSingleton();
	CharacterImporter::GetInstance()->Import();

	AssetManager::GetInstance()->LoadAsset(AssetType::Model, L"Assets/player-01.fbx");
	AssetManager::GetInstance()->LoadAsset(AssetType::Model, L"Assets/Arrow.fbx");
	AssetManager::GetInstance()->LoadAsset(AssetType::Model, L"Assets/field_1.fbx");
	AssetManager::GetInstance()->LoadAsset(AssetType::Model, L"Assets/Wall_A.fbx");
	AssetManager::GetInstance()->LoadAsset(AssetType::Model, L"Assets/Warrok-00.fbx");
	AssetManager::GetInstance()->LoadAsset(AssetType::Model, L"Assets/Warrok-01.fbx");
	AssetManager::GetInstance()->LoadAsset(AssetType::Texture, L"Assets/BaseTexture-00.jpg");
	AssetManager::GetInstance()->LoadAsset(AssetType::Texture, L"Assets/TextureNormal-00.jpg");
	AssetManager::GetInstance()->LoadAsset(AssetType::Texture, L"Assets/T_Wall_C.png");
	AssetManager::GetInstance()->LoadAsset(AssetType::Texture, L"Assets/T_Wall_N.png");

	// Worldにシステムを追加
	m_world.AddSystem(std::make_unique<GameManagerSystem>());
	m_world.AddSystem(std::make_unique<ComboSystem>());
	m_world.AddSystem(std::make_unique<GuardSystem>());
	m_world.AddSystem(std::make_unique<PlayerSystem>());
	m_world.AddSystem(std::make_unique<BattleCameraSystem>());
	m_world.AddSystem(std::make_unique<EnemyAISystem>());
	m_world.AddSystem(std::make_unique<EnemySystem>());
	m_world.AddSystem(std::make_unique<ArrowSystem>());

	return true;
}

void MainScene::Start()
{
	m_world.CreateWithLight(LightType::Directional, Color(0.7, 0.3, 0.6), 2000, nullptr, Vector3(1000, 250, 1000), Quaternion::Euler(90.0f, 0.0f, 0.0f));
	
	{
		Entity floor = m_world.CreateCube(500.0f, 1.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(floor)->position = Vector3(1250, 0, 1250);
	}
	{
		Entity floor = m_world.CreateCube(500.0f, 1.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(floor)->position = Vector3(750, 0, 1250);
	}
	{
		Entity floor = m_world.CreateCube(500.0f, 1.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(floor)->position = Vector3(250, 0, 1250);
	}
	{
		Entity floor = m_world.CreateCube(500.0f, 1.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(floor)->position = Vector3(-250, 0, 1250);
	}
	{
		Entity floor = m_world.CreateCube(500.0f, 1.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(floor)->position = Vector3(-750, 0, 1250);
	}
	{
		Entity floor = m_world.CreateCube(500.0f, 1.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(floor)->position = Vector3(-1250, 0, 1250);
	}

	{
		Entity floor = m_world.CreateCube(500.0f, 1.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(floor)->position = Vector3(1250, 0, 750);
	}
	{
		Entity floor = m_world.CreateCube(500.0f, 1.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(floor)->position = Vector3(750, 0, 750);
	}
	{
		Entity floor = m_world.CreateCube(500.0f, 1.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(floor)->position = Vector3(250, 0, 750);
	}
	{
		Entity floor = m_world.CreateCube(500.0f, 1.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(floor)->position = Vector3(-250, 0, 750);
	}
	{
		Entity floor = m_world.CreateCube(500.0f, 1.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(floor)->position = Vector3(-750, 0, 750);
	}
	{
		Entity floor = m_world.CreateCube(500.0f, 1.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(floor)->position = Vector3(-1250, 0, 750);
	}

	{
		Entity floor = m_world.CreateCube(500.0f, 1.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(floor)->position = Vector3(1250, 0, 250);
	}
	{
		Entity floor = m_world.CreateCube(500.0f, 1.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(floor)->position = Vector3(750, 0, 250);
	}
	{
		Entity floor = m_world.CreateCube(500.0f, 1.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(floor)->position = Vector3(250, 0, 250);
	}
	{
		Entity floor = m_world.CreateCube(500.0f, 1.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(floor)->position = Vector3(-250, 0, 250);
	}
	{
		Entity floor = m_world.CreateCube(500.0f, 1.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(floor)->position = Vector3(-750, 0, 250);
	}
	{
		Entity floor = m_world.CreateCube(500.0f, 1.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(floor)->position = Vector3(-1250, 0, 250);
	}

	{
		Entity floor = m_world.CreateCube(500.0f, 1.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(floor)->position = Vector3(1250, 0, -250);
	}
	{
		Entity floor = m_world.CreateCube(500.0f, 1.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(floor)->position = Vector3(750, 0, -250);
	}
	{
		Entity floor = m_world.CreateCube(500.0f, 1.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(floor)->position = Vector3(250, 0, -250);
	}
	{
		Entity floor = m_world.CreateCube(500.0f, 1.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(floor)->position = Vector3(-250, 0, -250);
	}
	{
		Entity floor = m_world.CreateCube(500.0f, 1.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(floor)->position = Vector3(-750, 0, -250);
	}
	{
		Entity floor = m_world.CreateCube(500.0f, 1.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(floor)->position = Vector3(-1250, 0, -250);
	}

	{
		Entity floor = m_world.CreateCube(500.0f, 1.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(floor)->position = Vector3(1250, 0, -750);
	}
	{
		Entity floor = m_world.CreateCube(500.0f, 1.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(floor)->position = Vector3(750, 0, -750);
	}
	{
		Entity floor = m_world.CreateCube(500.0f, 1.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(floor)->position = Vector3(250, 0, -750);
	}
	{
		Entity floor = m_world.CreateCube(500.0f, 1.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(floor)->position = Vector3(-250, 0, -750);
	}
	{
		Entity floor = m_world.CreateCube(500.0f, 1.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(floor)->position = Vector3(-750, 0, -750);
	}
	{
		Entity floor = m_world.CreateCube(500.0f, 1.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(floor)->position = Vector3(-1250, 0, -750);
	}

	{
		Entity floor = m_world.CreateCube(500.0f, 1.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(floor)->position = Vector3(1250, 0, -1250);
	}
	{
		Entity floor = m_world.CreateCube(500.0f, 1.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(floor)->position = Vector3(750, 0, -1250);
	}
	{
		Entity floor = m_world.CreateCube(500.0f, 1.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(floor)->position = Vector3(250, 0, -1250);
	}
	{
		Entity floor = m_world.CreateCube(500.0f, 1.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(floor)->position = Vector3(-250, 0, -1250);
	}
	{
		Entity floor = m_world.CreateCube(500.0f, 1.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(floor)->position = Vector3(-750, 0, -1250);
	}
	{
		Entity floor = m_world.CreateCube(500.0f, 1.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(floor)->position = Vector3(-1250, 0, -1250);
	}

	{
		Entity wall0 = m_world.CreateCube(500.0f, 500.0f, 10.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall0)->position = Vector3(1250, 250, 1500);
	}
	{
		Entity wall0 = m_world.CreateCube(500.0f, 500.0f, 10.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall0)->position = Vector3(250, 250, 1500);
	}
	{
		Entity wall0 = m_world.CreateCube(500.0f, 500.0f, 10.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall0)->position = Vector3(750, 250, 1500);
	}
	{
		Entity wall0 = m_world.CreateCube(500.0f, 500.0f, 10.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall0)->position = Vector3(-250, 250, 1500);
	}
	{
		Entity wall0 = m_world.CreateCube(500.0f, 500.0f, 10.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall0)->position = Vector3(-750, 250, 1500);
	}
	{
		Entity wall0 = m_world.CreateCube(500.0f, 500.0f, 10.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall0)->position = Vector3(-1250, 250, 1500);
	}

	{
		Entity wall1 = m_world.CreateCube(500.0f, 500.0f, 10.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall1)->position = Vector3(1250, 250, -1500);
	}
	{
		Entity wall1 = m_world.CreateCube(500.0f, 500.0f, 10.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall1)->position = Vector3(250, 250, -1500);
	}
	{
		Entity wall1 = m_world.CreateCube(500.0f, 500.0f, 10.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall1)->position = Vector3(750, 250, -1500);
	}
	{
		Entity wall1 = m_world.CreateCube(500.0f, 500.0f, 10.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall1)->position = Vector3(-250, 250, -1500);
	}
	{
		Entity wall1 = m_world.CreateCube(500.0f, 500.0f, 10.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall1)->position = Vector3(-750, 250, -1500);
	}
	{
		Entity wall1 = m_world.CreateCube(500.0f, 500.0f, 10.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall1)->position = Vector3(-1250, 250, -1500);
	}

	{
		Entity wall2 = m_world.CreateCube(10.0f, 500.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall2)->position = Vector3(1500, 250, 1250);
	}
	{
		Entity wall2 = m_world.CreateCube(10.0f, 500.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall2)->position = Vector3(1500, 250, 250);
	}
	{
		Entity wall2 = m_world.CreateCube(10.0f, 500.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall2)->position = Vector3(1500, 250, 750);
	}
	{
		Entity wall2 = m_world.CreateCube(10.0f, 500.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall2)->position = Vector3(1500, 250, -250);
	}
	{
		Entity wall2 = m_world.CreateCube(10.0f, 500.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall2)->position = Vector3(1500, 250, -750);
	}
	{
		Entity wall2 = m_world.CreateCube(10.0f, 500.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall2)->position = Vector3(1500, 250, -1250);
	}

	{
		Entity wall3 = m_world.CreateCube(10.0f, 500.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall3)->position = Vector3(-1500, 250, 1250);
	}
	{
		Entity wall3 = m_world.CreateCube(10.0f, 500.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall3)->position = Vector3(-1500, 250, 250);
	}
	{
		Entity wall3 = m_world.CreateCube(10.0f, 500.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall3)->position = Vector3(-1500, 250, 750);
	}
	{
		Entity wall3 = m_world.CreateCube(10.0f, 500.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall3)->position = Vector3(-1500, 250, -250);
	}
	{
		Entity wall3 = m_world.CreateCube(10.0f, 500.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall3)->position = Vector3(-1500, 250, -750);
	}
	{
		Entity wall3 = m_world.CreateCube(10.0f, 500.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall3)->position = Vector3(-1500, 250, -1250);
	}

	// 壁の一つ手前にコライダーを生成
	{
		Entity wallCollider = m_world.CreateCube(5000.0f, 500.0f, 10.0f, Layers::Environment, Color::white, true);
		m_world.GetComponent<Transform>(wallCollider)->position = Vector3(1000, 250, 1000);
	}
	{
		Entity wallCollider = m_world.CreateCube(10.0f, 500.0f, 5000.0f, Layers::Environment, Color::white, true);
		m_world.GetComponent<Transform>(wallCollider)->position = Vector3(1000, 250, 1000);
	}
	{
		Entity wallCollider = m_world.CreateCube(5000.0f, 500.0f, 10.0f, Layers::Environment, Color::white, true);
		m_world.GetComponent<Transform>(wallCollider)->position = Vector3(-1000, 250, -1000);
	}
	{
		Entity wallCollider = m_world.CreateCube(10.0f, 500.0f, 5000.0f, Layers::Environment, Color::white, true);
		m_world.GetComponent<Transform>(wallCollider)->position = Vector3(-1000, 250, -1000);
	}
}

void MainScene::Update()
{	
}

void MainScene::Draw()
{
}