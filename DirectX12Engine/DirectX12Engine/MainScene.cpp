#include "MainScene.h"
#include "SystemList.h"
#include "DebugManager.h"
#include "ArrowSystem.h"

bool MainScene::Load()
{
	AssetManager::GetInstance()->LoadAsset(AssetType::Model, L"Assets/player.fbx");
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
	m_world.AddSystem(std::make_unique<PlayerSystem>());
	m_world.AddSystem(std::make_unique<PlayerCameraSystem>());
	m_world.AddSystem(std::make_unique<EnemySystem>());
	m_world.AddSystem(std::make_unique<ArrowSystem>());

	return true;
}

void MainScene::Start()
{
	m_world.CreateWithLight(LightType::Directional, Color(0.3, 0.1, 0.2), 2000, nullptr, Vector3(1000, 250, 1000), Quaternion::Euler(90.0f, 0.0f, 0.0f));
	
	m_world.CreateWithLight(LightType::Point, Color(200, 100, 100), 750, nullptr, Vector3(980, 250, 180));
	m_world.CreateWithLight(LightType::Point, Color(200, 100, 100), 750, nullptr, Vector3(980, 250, 480));
	m_world.CreateWithLight(LightType::Point, Color(200, 100, 100), 750, nullptr, Vector3(980, 250, 980));
	m_world.CreateWithLight(LightType::Point, Color(200, 100, 100), 750, nullptr, Vector3(580, 250, 980));
	m_world.CreateWithLight(LightType::Point, Color(200, 100, 100), 750, nullptr, Vector3(180, 250, 980));
	m_world.CreateWithLight(LightType::Point, Color(200, 100, 100), 750, nullptr, Vector3(-180, 250, 980));
	m_world.CreateWithLight(LightType::Point, Color(200, 100, 100), 750, nullptr, Vector3(-580, 250, 980));
	m_world.CreateWithLight(LightType::Point, Color(200, 100, 100), 750, nullptr, Vector3(-980, 250, 980));
	m_world.CreateWithLight(LightType::Point, Color(200, 100, 100), 750, nullptr, Vector3(-980, 250, 580));
	m_world.CreateWithLight(LightType::Point, Color(200, 100, 100), 750, nullptr, Vector3(-980, 250, 180));
	m_world.CreateWithLight(LightType::Point, Color(200, 100, 100), 750, nullptr, Vector3(-980, 250, -180));
	m_world.CreateWithLight(LightType::Point, Color(200, 100, 100), 750, nullptr, Vector3(-980, 250, -480));
	m_world.CreateWithLight(LightType::Point, Color(200, 100, 100), 750, nullptr, Vector3(-980, 250, -980));
	m_world.CreateWithLight(LightType::Point, Color(200, 100, 100), 750, nullptr, Vector3(-580, 250, -980));
	m_world.CreateWithLight(LightType::Point, Color(200, 100, 100), 750, nullptr, Vector3(-180, 250, -980));
	m_world.CreateWithLight(LightType::Point, Color(200, 100, 100), 750, nullptr, Vector3(180, 250, -980));
	m_world.CreateWithLight(LightType::Point, Color(200, 100, 100), 750, nullptr, Vector3(580, 250, -980));
	m_world.CreateWithLight(LightType::Point, Color(200, 100, 100), 750, nullptr, Vector3(980, 250, -980));
	m_world.CreateWithLight(LightType::Point, Color(200, 100, 100), 750, nullptr, Vector3(980, 250, -480));
	m_world.CreateWithLight(LightType::Point, Color(200, 100, 100), 750, nullptr, Vector3(980, 250, -180));

	
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
		Entity wall0 = m_world.CreateCube(500.0f, 500.0f, 1.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall0)->position = Vector3(250, 250, 1000);
	}
	{
		Entity wall0 = m_world.CreateCube(500.0f, 500.0f, 1.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall0)->position = Vector3(750, 250, 1000);
	}
	{
		Entity wall0 = m_world.CreateCube(500.0f, 500.0f, 1.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall0)->position = Vector3(-250, 250, 1000);
	}
	{
		Entity wall0 = m_world.CreateCube(500.0f, 500.0f, 1.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall0)->position = Vector3(-750, 250, 1000);
	}

	{
		Entity wall1 = m_world.CreateCube(500.0f, 500.0f, 1.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall1)->position = Vector3(250, 250, -1000);
	}
	{
		Entity wall1 = m_world.CreateCube(500.0f, 500.0f, 1.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall1)->position = Vector3(750, 250, -1000);
	}
	{
		Entity wall1 = m_world.CreateCube(500.0f, 500.0f, 1.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall1)->position = Vector3(-250, 250, -1000);
	}
	{
		Entity wall1 = m_world.CreateCube(500.0f, 500.0f, 1.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall1)->position = Vector3(-750, 250, -1000);
	}

	{
		Entity wall2 = m_world.CreateCube(1.0f, 500.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall2)->position = Vector3(1000, 250, 250);
	}
	{
		Entity wall2 = m_world.CreateCube(1.0f, 500.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall2)->position = Vector3(1000, 250, 750);
	}
	{
		Entity wall2 = m_world.CreateCube(1.0f, 500.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall2)->position = Vector3(1000, 250, -250);
	}
	{
		Entity wall2 = m_world.CreateCube(1.0f, 500.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall2)->position = Vector3(1000, 250, -750);
	}

	{
		Entity wall3 = m_world.CreateCube(1.0f, 500.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall3)->position = Vector3(-1000, 250, 250);
	}
	{
		Entity wall3 = m_world.CreateCube(1.0f, 500.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall3)->position = Vector3(-1000, 250, 750);
	}
	{
		Entity wall3 = m_world.CreateCube(1.0f, 500.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall3)->position = Vector3(-1000, 250, -250);
	}
	{
		Entity wall3 = m_world.CreateCube(1.0f, 500.0f, 500.0f, Layers::Environment, Color::white, false, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png"));
		m_world.GetComponent<Transform>(wall3)->position = Vector3(-1000, 250, -750);
	}
}

void MainScene::Update()
{	
}

void MainScene::Draw()
{
}