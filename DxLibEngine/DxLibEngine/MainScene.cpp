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
	m_world.AddSystem(std::make_unique<CollisionSystem>());

	float fov = 60.0f;
	float aspect = (float)Screen::GetWidth() / (float)Screen::GetHeight();
	float nearPlane = 0.1f;
	float farPlane = 1000.0f;

	Vector3 cameraPos = Vector3(0.0f, 1.0f, -8.0f);
	Entity* camera = m_world.CreateCamera3D(fov, aspect, nearPlane, farPlane, cameraPos);
	// 騎士の顔が見えるように、カメラの向きを少しだけ下げる
	m_world.GetComponent<Transform>(*camera)->rotation = Quaternion::AngleAxis(5.0f, Vector3::right);

	m_world.CreateWithModel(L"Assets/Warrok-01.fbx", nullptr, Vector3(0, -100, 300), Quaternion::identity);

	// 指向性ライト（Directional Light）の作成 (太陽光のような役割)
	{
		Entity* directionalLightEntity = m_world.CreateEntity();
	
		Light light;
		light.type = LightType::Directional;
		light.color = Color(1.0f, 0.9f, 0.7f, 1.0f); // 少し暖かい白色
		m_world.AddComponent<Light>(*directionalLightEntity, light);
	
		// ライトの向きをTransformの回転で制御する
		Transform* lightTransform = m_world.GetComponent<Transform>(*directionalLightEntity);
		// 右斜め上から照らすように回転させる
		lightTransform->rotation = Quaternion::Euler(90.0f, 0.0f, 90.0f);
		lightTransform->position = Vector3(0, 1000, -300);
	}

	// 点光源（Point Light）の作成 (モデルの左側から照らす)
	{
    Entity* pointLightEntity = m_world.CreateEntity();
	
    Light light;
    light.type = LightType::Point; // タイプを Point に設定
    light.color = Color(0.0f, 0.5f, 1.0f, 1.0f); // 識別しやすいように青色に
    light.range = 1000.0f; // 光の届く範囲を十分に確保
	
    // ポイントライトには向きは関係ないため、回転やspotCosAngleの設定は不要
    m_world.AddComponent<Light>(*pointLightEntity, light);
    
    // ライトの位置を設定
    Transform* lightTransform = m_world.GetComponent<Transform>(*pointLightEntity);
    // モデルの少し手前、左上に配置
    lightTransform->position = Vector3(0, -150, -280);
	}

	m_world.Start(m_world);
}

void MainScene::Update()
{
	View<Transform, SkinnedMeshRenderer> view(m_world.GetComponentManager());
	for (auto [entity, transform, renderer] : view)
	{
		m_world.GetSystem<TransformSystem>()->Rotate(transform, Vector3(0, 1, 0), 15.0f * Time::GetDeltaTime());
	}

	m_world.Update(m_world);
}

void MainScene::Draw()
{
	m_world.Draw(m_world);
}
