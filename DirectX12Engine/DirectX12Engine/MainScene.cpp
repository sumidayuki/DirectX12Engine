#include "MainScene.h"
#include "SystemList.h"

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
		Entity directionalLightEntity = m_world.CreateEntity();
	
		Light light;
		light.type = LightType::Directional;
		light.color = Color(1.0f, 0.7f, 0.9f, 1.0f);
		m_world.AddComponent<Light>(directionalLightEntity, light);
	
		// ライトの向きをTransformの回転で制御する
		Transform* lightTransform = m_world.GetComponent<Transform>(directionalLightEntity);
		// 右斜め上から照らすように回転させる
		lightTransform->rotation = Quaternion::Euler(-90.0f, 0.0f, 0.0f);
		lightTransform->position = Vector3(0, 5000, 0);
	}

	// 点光源（Point Light）の作成 (モデルの左側から照らす)
	{
    Entity pointLightEntity = m_world.CreateEntity();
	
    Light light;
    light.type = LightType::Point; // タイプを Point に設定
	light.color = Color(1.0f, 0.9f, 0.7f, 1.0f); // 少し暖かい白色
	light.range = 1000.0f; // 光の届く範囲を十分に確保
	
    // ポイントライトには向きは関係ないため、回転やspotCosAngleの設定は不要
    m_world.AddComponent<Light>(pointLightEntity, light);
    
    // ライトの位置を設定
    Transform* lightTransform = m_world.GetComponent<Transform>(pointLightEntity);
	
	lightTransform->rotation = Quaternion::Euler(0, 45, 0);
    
	// モデルの少し手前、左上に配置
    lightTransform->position = Vector3(-1000, 0, -1000);
	}
	
	// 点光源（Point Light）の作成 (モデルの左側から照らす)
	{
		Entity pointLightEntity = m_world.CreateEntity();
	
		Light light;
		light.type = LightType::Point; // タイプを Point に設定
		light.color = Color(1.0f, 0.9f, 0.7f, 1.0f); // 少し暖かい白色
		light.range = 2000.0f; // 光の届く範囲を十分に確保
	
		// ポイントライトには向きは関係ないため、回転やspotCosAngleの設定は不要
		m_world.AddComponent<Light>(pointLightEntity, light);
	
		// ライトの位置を設定
		Transform* lightTransform = m_world.GetComponent<Transform>(pointLightEntity);
	
		lightTransform->rotation = Quaternion::LookRotation(Vector3(0, -135, 0) - lightTransform->position);
	
		// モデルの少し手前、左上に配置
		lightTransform->position = Vector3(1000, 0, 1000);
	}
	
	// 点光源（Point Light）の作成 (モデルの左側から照らす)
	{
		Entity pointLightEntity = m_world.CreateEntity();
	
		Light light;
		light.type = LightType::Point; // タイプを Point に設定
		light.color = Color(1.0f, 0.9f, 0.7f, 1.0f); // 少し暖かい白色
		light.range = 2000.0f; // 光の届く範囲を十分に確保
	
		// ポイントライトには向きは関係ないため、回転やspotCosAngleの設定は不要
		m_world.AddComponent<Light>(pointLightEntity, light);
	
		// ライトの位置を設定
		Transform* lightTransform = m_world.GetComponent<Transform>(pointLightEntity);
	
		lightTransform->rotation = Quaternion::LookRotation(Vector3(0, 135, 0) - lightTransform->position);
	
		// モデルの少し手前、左上に配置
		lightTransform->position = Vector3(-1000, 0, 1000);
	}
	
	// 点光源（Point Light）の作成 (モデルの左側から照らす)
	{
		Entity pointLightEntity = m_world.CreateEntity();
	
		Light light;
		light.type = LightType::Point; // タイプを Point に設定
		light.color = Color(1.0f, 0.9f, 0.7f, 1.0f); // 少し暖かい白色
		light.range = 2000.0f; // 光の届く範囲を十分に確保
	
		// ポイントライトには向きは関係ないため、回転やspotCosAngleの設定は不要
		m_world.AddComponent<Light>(pointLightEntity, light);
	
		// ライトの位置を設定
		Transform* lightTransform = m_world.GetComponent<Transform>(pointLightEntity);
	
		lightTransform->rotation = Quaternion::LookRotation(Vector3(0, -45, 0) - lightTransform->position);
	
		// モデルの少し手前、左上に配置
		lightTransform->position = Vector3(1000, 0, -1000);
	}

	Entity floor = m_world.CreateWithModel(L"Assets/floor-01.fbx", nullptr, Vector3(0, 0, 0), Quaternion::Euler(0, 0, 0));	
}

void MainScene::Update()
{	

}

void MainScene::Draw()
{
}