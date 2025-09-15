#include "MainScene.h"
#include "SystemList.h"

void MainScene::Start()
{
	// Worldにシステムを追加
	m_world.AddSystem(std::make_unique<CameraSystem>());
	m_world.AddSystem(std::make_unique<SpriteRendererSystem>());
	m_world.AddSystem(std::make_unique<MeshRendererSystem>());
	m_world.AddSystem(std::make_unique<SkinnedMeshRendererSystem>());
	m_world.AddSystem(std::make_unique<TransformSystem>());
	m_world.AddSystem(std::make_unique<AnimationSystem>());
	m_world.AddSystem(std::make_unique<LightSystem>());
	m_world.AddSystem(std::make_unique<InputSystem>());
	m_world.AddSystem(std::make_unique<PlayerSystem>());
	m_world.AddSystem(std::make_unique<PlayerCameraSystem>());

	m_model = m_world.CreateWithModel(L"Assets/Warrok-00.fbx", nullptr, Vector3(300, -100, 500), Quaternion::identity);

	m_animator = m_world.GetComponent<Animator>(*m_model);
	
	m_animator->isLoop = false;

	////指向性ライト（Directional Light）の作成 (太陽光のような役割)
	//{
	//	Entity* directionalLightEntity = m_world.CreateWithModel(warrokModel.Get(), nullptr, Vector3(0, 0, 0), Quaternion::identity);
	//
	//	Light light;
	//	light.type = LightType::Directional;
	//	light.color = Color(1.0f, 0.9f, 0.7f, 1.0f); // 少し暖かい白色
	//	m_world.AddComponent<Light>(*directionalLightEntity, light);
	//
	//	// ライトの向きをTransformの回転で制御する
	//	Transform* lightTransform = m_world.GetComponent<Transform>(*directionalLightEntity);
	//	// 右斜め上から照らすように回転させる
	//	lightTransform->rotation = Quaternion::Euler(90.0f, 180.0f, 0.0f);
	//	lightTransform->position = Vector3(0, 2000, 0);
	//}

	// 点光源（Point Light）の作成 (モデルの左側から照らす)
	{
    Entity* pointLightEntity = m_world.CreateEntity();
	
    Light light;
    light.type = LightType::Point; // タイプを Point に設定
	light.color = Color(1.0f, 0.9f, 0.7f, 1.0f); // 少し暖かい白色
	light.range = 1000.0f; // 光の届く範囲を十分に確保
	
    // ポイントライトには向きは関係ないため、回転やspotCosAngleの設定は不要
    m_world.AddComponent<Light>(*pointLightEntity, light);
    
    // ライトの位置を設定
    Transform* lightTransform = m_world.GetComponent<Transform>(*pointLightEntity);
	
	lightTransform->rotation = Quaternion::Euler(0, 45, 0);
    
	// モデルの少し手前、左上に配置
    lightTransform->position = Vector3(-1000, 0, -1000);
	}

	// 点光源（Point Light）の作成 (モデルの左側から照らす)
	{
		Entity* pointLightEntity = m_world.CreateEntity();

		Light light;
		light.type = LightType::Point; // タイプを Point に設定
		light.color = Color(1.0f, 0.9f, 0.7f, 1.0f); // 少し暖かい白色
		light.range = 2000.0f; // 光の届く範囲を十分に確保

		// ポイントライトには向きは関係ないため、回転やspotCosAngleの設定は不要
		m_world.AddComponent<Light>(*pointLightEntity, light);

		// ライトの位置を設定
		Transform* lightTransform = m_world.GetComponent<Transform>(*pointLightEntity);

		lightTransform->rotation = Quaternion::LookRotation(Vector3(0, -135, 0) - lightTransform->position);

		// モデルの少し手前、左上に配置
		lightTransform->position = Vector3(1000, 0, 1000);
	}

	// 点光源（Point Light）の作成 (モデルの左側から照らす)
	{
		Entity* pointLightEntity = m_world.CreateEntity();

		Light light;
		light.type = LightType::Point; // タイプを Point に設定
		light.color = Color(1.0f, 0.9f, 0.7f, 1.0f); // 少し暖かい白色
		light.range = 2000.0f; // 光の届く範囲を十分に確保

		// ポイントライトには向きは関係ないため、回転やspotCosAngleの設定は不要
		m_world.AddComponent<Light>(*pointLightEntity, light);

		// ライトの位置を設定
		Transform* lightTransform = m_world.GetComponent<Transform>(*pointLightEntity);

		lightTransform->rotation = Quaternion::LookRotation(Vector3(0, 135, 0) - lightTransform->position);

		// モデルの少し手前、左上に配置
		lightTransform->position = Vector3(-1000, 0, 1000);
	}

	// 点光源（Point Light）の作成 (モデルの左側から照らす)
	{
		Entity* pointLightEntity = m_world.CreateEntity();

		Light light;
		light.type = LightType::Point; // タイプを Point に設定
		light.color = Color(1.0f, 0.9f, 0.7f, 1.0f); // 少し暖かい白色
		light.range = 2000.0f; // 光の届く範囲を十分に確保

		// ポイントライトには向きは関係ないため、回転やspotCosAngleの設定は不要
		m_world.AddComponent<Light>(*pointLightEntity, light);

		// ライトの位置を設定
		Transform* lightTransform = m_world.GetComponent<Transform>(*pointLightEntity);

		lightTransform->rotation = Quaternion::LookRotation(Vector3(0, -45, 0) - lightTransform->position);

		// モデルの少し手前、左上に配置
		lightTransform->position = Vector3(1000, 0, -1000);
	}
}

void MainScene::Update()
{	
	if (Keyboard::GetKeyState(KeyCode::Enter).WasPressedThisFrame())
	{
		if (m_animator)
		{
			// 現在再生中のアニメーションに応じて切り替え
			if (m_animator->currentClipName == "Tpose")
			{
				AnimationSystem::Play(*m_animator, "Swing");
			}
			else
			{
				AnimationSystem::Play(*m_animator, "Tpose");
			}
		}
	}
}

void MainScene::Draw()
{
}