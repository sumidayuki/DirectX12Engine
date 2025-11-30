#include "MainScene.h"
#include "SystemList.h"
#include "DebugManager.h"

bool MainScene::Load()
{
	AssetManager::GetInstance()->LoadAsset(AssetType::Model, L"Assets/player.fbx");
	AssetManager::GetInstance()->LoadAsset(AssetType::Model, L"Assets/Arrow.fbx");
	AssetManager::GetInstance()->LoadAsset(AssetType::Model, L"Assets/floor-01.fbx");
	AssetManager::GetInstance()->LoadAsset(AssetType::Model, L"Assets/Warrok-00.fbx");

	// Worldにシステムを追加
	m_world.AddSystem(std::make_unique<GameManagerSystem>());
	m_world.AddSystem(std::make_unique<PlayerSystem>());
	m_world.AddSystem(std::make_unique<PlayerCameraSystem>());
	m_world.AddSystem(std::make_unique<EnemySystem>());

	return true;
}

void MainScene::Start()
{
	//指向性ライト（Directional Light）の作成 (太陽光のような役割)
	{
		Entity directionalLightEntity = m_world.CreateSphere(500, 16, 16);
	
		Light light;
		light.type = LightType::Directional;
		light.color = Color(1.0f, 1.0f, 0.9f, 1.0f);
		m_world.AddComponent<Light>(directionalLightEntity, light);
	
		// ライトの向きをTransformの回転で制御する
		Transform* lightTransform = m_world.GetComponent<Transform>(directionalLightEntity);
		// 右斜め上から照らすように回転させる
		lightTransform->rotation = Quaternion::Euler(90.0f, 0.0f, 0.0f);
		lightTransform->position = Vector3(0, 2000, 0);
	}

	Entity floor = m_world.CreateWithModel(L"Assets/floor-01.fbx", nullptr, Vector3::zero, Quaternion::identity, Layers::Environment);
	
	Entity wall0 = m_world.CreateCube(2000.0f, 500.0f, 1.0f, Layers::Environment, Color::yellow, false);
	m_world.GetComponent<Transform>(wall0)->position = Vector3(0, 250, 1000);
	
	Entity wall1 = m_world.CreateCube(2000.0f, 500.0f, 1.0f, Layers::Environment, Color::yellow, false);
	m_world.GetComponent<Transform>(wall1)->position = Vector3(0, 250, -1000);
	
	Entity wall2 = m_world.CreateCube(1.0f, 500.0f, 2000.0f, Layers::Environment, Color::yellow, false);
	m_world.GetComponent<Transform>(wall2)->position = Vector3(1000, 250, 0);
	
	Entity wall3 = m_world.CreateCube(1.0f, 500.0f, 2000.0f, Layers::Environment, Color::yellow, false);
	m_world.GetComponent<Transform>(wall3)->position = Vector3(-1000, 250, 0);
}

void MainScene::Update()
{	
}

void MainScene::Draw()
{
}