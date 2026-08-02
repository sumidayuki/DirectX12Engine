#include "MainScene.h"
#include "SystemList.h"
#include "DebugManager.h"
#include "ArrowSystem.h"
#include "CharacterImporter.h"
#include "UILayoutImporter.h"
#include "AIRuleImporter.h"

bool MainScene::Load()
{
	CharacterInfoRegistry::CreateSingleton();
	CharacterImporter characterImporter;
	characterImporter.Import();

	AIRuleImporter aiImporter;
	aiImporter.Import();

	AssetManager::GetInstance()->LoadAsset(AssetType::Model, L"Assets/Archer.fbx");
	AssetManager::GetInstance()->LoadAsset(AssetType::Model, L"Assets/Arrow.fbx");
	AssetManager::GetInstance()->LoadAsset(AssetType::Model, L"Assets/field_1.fbx");
	AssetManager::GetInstance()->LoadAsset(AssetType::Model, L"Assets/Wall_A.fbx");
	AssetManager::GetInstance()->LoadAsset(AssetType::Model, L"Assets/Warrok.fbx");
	AssetManager::GetInstance()->LoadAsset(AssetType::Texture, L"Assets/BaseTexture-00.jpg");
	AssetManager::GetInstance()->LoadAsset(AssetType::Texture, L"Assets/TextureNormal-00.jpg");
	AssetManager::GetInstance()->LoadAsset(AssetType::Texture, L"Assets/T_Wall_C.png");
	AssetManager::GetInstance()->LoadAsset(AssetType::Texture, L"Assets/T_Wall_N.png");
	AssetManager::GetInstance()->LoadAssetsFromDirectory(AssetType::Texture, L"Assets/Images/InputDevice_Icons/Key_Icons");
	AssetManager::GetInstance()->LoadAssetsFromDirectory(AssetType::Texture, L"Assets/Images/InputDevice_Icons/Gamepad_Icons/xbox");
	AssetManager::GetInstance()->LoadAssetsFromDirectory(AssetType::Texture, L"Assets/Images/InputDevice_Icons/Mouse_Icons");
	AssetManager::GetInstance()->LoadAssetsFromDirectory(AssetType::Texture, L"Assets/Images/Text");
	AssetManager::GetInstance()->LoadAsset(AssetType::Audio, L"Assets/Audio/SE/Archer/se-foot.wav");
	AssetManager::GetInstance()->LoadAsset(AssetType::Audio, L"Assets/Audio/SE/Archer/se-guard.wav");
	AssetManager::GetInstance()->LoadAsset(AssetType::Audio, L"Assets/Audio/SE/Archer/se-kick.wav");
	AssetManager::GetInstance()->LoadAsset(AssetType::Audio, L"Assets/Audio/SE/Archer/se-shageki.wav");
	AssetManager::GetInstance()->LoadAsset(AssetType::Audio, L"Assets/Audio/SE/Archer/se-damage.wav");
	AssetManager::GetInstance()->LoadAsset(AssetType::Audio, L"Assets/Audio/SE/Warrok/se-punch.wav");
	AssetManager::GetInstance()->LoadAsset(AssetType::Audio, L"Assets/Audio/SE/Warrok/se-jump.wav");


	// Worldにシステムを追加
	m_world.AddSystem(std::make_unique<GameManagerSystem>());
	m_world.AddSystem(std::make_unique<StaminaSystem>());
	m_world.AddSystem(std::make_unique<GuardSystem>());
	m_world.AddSystem(std::make_unique<MoveSystem>());
	m_world.AddSystem(std::make_unique<PlayerActionGuideSystem>());
	m_world.AddSystem(std::make_unique<PlayerSystem>());
	m_world.AddSystem(std::make_unique<PlayerCameraSystem>());
	m_world.AddSystem(std::make_unique<BattleCameraSystem>());
	m_world.AddSystem(std::make_unique<AIStateMachineSystem>());
	m_world.AddSystem(std::make_unique<EnemySystem>());
	m_world.AddSystem(std::make_unique<ArrowSystem>());

	UILayoutImporter importer;
	importer.Import(L"Assets/Json/UI/MainSceneUI.json", m_world);

	return true;
}

void MainScene::Start()
{
	m_world.CreateWithLight(LightType::Directional, Color(0.7, 0.3, 0.6), 2000, nullptr, Vector3(1000, 250, 1000), Quaternion::Euler(90.0f, 0.0f, 0.0f));
	
	Texture2D* wallTexture = AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_C.png");

	// 座標の基準リスト
	const float coords[] = { 1250.0f, 750.0f, 250.0f, -250.0f, -750.0f, -1250.0f };

	// 床の生成
	for (float z : coords)
	{
		for (float x : coords)
		{
			Entity floor = m_world.CreateCube(500.0f, 1.0f, 500.0f, Layers::Environment, Color::white, false, wallTexture);
			m_world.GetComponent<Transform>(floor)->position = Vector3(x, 0.0f, z);
		}
	}

	// 壁の生成
	for (float pos : coords)
	{
		// Z = 1500 の壁 (手前)
		Entity wall0 = m_world.CreateCube(500.0f, 500.0f, 10.0f, Layers::Environment, Color::white, false, wallTexture);
		m_world.GetComponent<Transform>(wall0)->position = Vector3(pos, 250.0f, 1500.0f);

		// Z = -1500 の壁 (奥)
		Entity wall1 = m_world.CreateCube(500.0f, 500.0f, 10.0f, Layers::Environment, Color::white, false, wallTexture);
		m_world.GetComponent<Transform>(wall1)->position = Vector3(pos, 250.0f, -1500.0f);

		// X = 1500 の壁 (右)
		Entity wall2 = m_world.CreateCube(10.0f, 500.0f, 500.0f, Layers::Environment, Color::white, false, wallTexture);
		m_world.GetComponent<Transform>(wall2)->position = Vector3(1500.0f, 250.0f, pos);

		// X = -1500 の壁 (左)
		Entity wall3 = m_world.CreateCube(10.0f, 500.0f, 500.0f, Layers::Environment, Color::white, false, wallTexture);
		m_world.GetComponent<Transform>(wall3)->position = Vector3(-1500.0f, 250.0f, pos);
	}

	Mouse::SetVisible(false);
	Mouse::SetLock(true);
}

void MainScene::Update()
{
}

void MainScene::Draw()
{
}