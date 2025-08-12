#include "World.h"

Entity* World::CreateEntity()
{
	// Entity を entityManager を使用してエンティティを生成します。
	Entity* entity = m_em.CreateEntity();

	// Entity に Transform コンポーネントを付けておきます。
	m_cm.AddComponent<Transform>(*entity, Transform{});

	return entity;
}

Entity* World::CreateWithSprite(const wchar_t* path, const Rect& rect, const Vector2 pivot, float pixelsPerUnit, Transform* parent, const Vector3& localPosition, const Quaternion& localRotation)
{
	TextureImporter importer;

	// テクスチャのロード
	ComPtr<Texture2D> texture;
	texture.Attach(importer.Import(path));

	return CreateWithSprite(texture.Get(), rect, pivot, pixelsPerUnit, parent, localPosition, localRotation);
}

Entity* World::CreateWithSprite(Texture2D* texture, const Rect& rect, const Vector2 pivot, float pixelsPerUnit, Transform* parent, const Vector3& localPosition, const Quaternion& localRotation)
{
	// スプライトの作成
	ComPtr<Sprite> sprite;
	sprite.Attach(Sprite::Create(texture, rect, pivot, pixelsPerUnit, pixelsPerUnit));

	return CreateWithSprite(sprite.Get(), parent, localPosition, localRotation);
}

Entity* World::CreateWithSprite(Sprite* sprite, Transform* parent, const Vector3& localPosition, const Quaternion& localRotation)
{
	Entity* entity = CreateEntity();

	SpriteRenderer* renderer = new SpriteRenderer();
	GetSystem<SpriteRendererSystem>()->SetSprite(renderer, sprite);
	AddComponent<SpriteRenderer>(*entity, *renderer);

	TransformSystem* transformSystem = GetSystem<TransformSystem>();
	Transform* transform = GetComponent<Transform>(*entity);
	transformSystem->SetLocalPosition(*transform, localPosition);
	transformSystem->SetLocalRotation(*transform, localRotation);

	return entity;
}

Entity* World::CreateCamera2D(float viewWidth, float viewHeight, const Vector3& localPosition, const Quaternion& localRotation)
{
	// 2D向けカメラの作成
	Entity* entity = CreateEntity();
	AddComponent<Camera>(*entity, Camera{});
	Camera* camera = GetComponent<Camera>(*entity);
	camera->orthographic = true;
	camera->orthographicSize = viewHeight * 0.5f;
	camera->clearFlags = CameraClearFlags::SolidColor;
	camera->backgroundColor = Color::cornflowerBlue;
	camera->aspect = viewWidth / viewHeight;
	camera->nearClipPlane = 0.3f;
	camera->farClipPlane = 1000.0f;
	camera->viewportRect = Rect(0.0f, 0.0f, 1.0f, 1.0f);

	TransformSystem* transformSystem = GetSystem<TransformSystem>();
	Transform* transform = GetComponent<Transform>(*entity);
	transformSystem->SetLocalPosition(*transform, localPosition);
	transformSystem->SetLocalRotation(*transform, localRotation);

	m_allCameras.push_back(camera);
	m_allCameraEntities.push_back(entity);
	return entity;
}

Entity* World::CreateWithModel(
	const Model* modelData,
	Transform* parent,
	const Vector3& localPosition,
	const Quaternion& localRotation)
{
	//基本的なエンティティを作成（Transformコンポーネントが自動で付与される）
	Entity* entity = CreateEntity();

	// MeshRendererコンポーネントを作成し、メッシュとマテリアルを設定
	MeshRenderer renderer;
	renderer.meshes = modelData->m_meshes;
	renderer.materials = modelData->m_materials;
	AddComponent<MeshRenderer>(*entity, renderer);

	// Transformを設定
	TransformSystem* transformSystem = GetSystem<TransformSystem>();
	Transform* transform = GetComponent<Transform>(*entity);
	transformSystem->SetLocalPosition(*transform, localPosition);
	transformSystem->SetLocalRotation(*transform, localRotation);
	// TODO: parentの設定処理を追加

	return entity;
}

Entity* World::CreateWithModel(const std::wstring& path, Transform* parent, const Vector3& localPosition, const Quaternion& localRotation)
{
	// 1. 新しいModelImporterを使ってモデルデータを読み込む
	ModelImporter importer;
	ComPtr<Model> modelData = importer.Import(path, *this);

	if (!modelData)
	{
		// 読み込みに失敗した場合
		std::wstring err_msg = L"Failed to load model: " + path + L"\n";
		OutputDebugStringW(err_msg.c_str());
		return nullptr;
	}

	// 2. 読み込んだデータを使って、コアとなる生成関数を呼び出す
	return CreateWithModel(modelData.Get(), parent, localPosition, localRotation);
}

Entity* World::CreateCamera3D(float fieldOfView, float aspect, float nearClipPlane, float farClipPlane, const Vector3& localPosition, const Quaternion& localRotation)
{
	// 3D向けカメラの作成
	Entity* entity = CreateEntity();
	AddComponent<Camera>(*entity, Camera{});
	Camera* camera = GetComponent<Camera>(*entity);

	camera->orthographic = false;							// 3Dなので orthographic は false にする
	camera->fieldOfView = fieldOfView;						// 視野角
	camera->aspect = aspect;								// アスペクト比
	camera->nearClipPlane = nearClipPlane;					// 近平面
	camera->farClipPlane = farClipPlane;					// 遠平面
	camera->clearFlags = CameraClearFlags::SolidColor;		// 背景のクリア方法
	camera->backgroundColor = Color::cornflowerBlue;		// 背景色
	camera->viewportRect = Rect(0.0f, 0.0f, 1.0f, 1.0f);

	TransformSystem* transformSystem = GetSystem<TransformSystem>();
	Transform* transform = GetComponent<Transform>(*entity);
	transformSystem->SetLocalPosition(*transform, localPosition);
	transformSystem->SetLocalRotation(*transform, localRotation);

	m_allCameras.push_back(camera);
	m_allCameraEntities.push_back(entity);

	return entity;
}

void World::DestoryEntity(Entity* entity)
{
	if (!entity || !m_em.IsAlive(entity)) return; // ポインタが有効かチェック

	m_cm.RemoveAllComponents(*entity);
	m_em.DestroyEntity(entity);

	delete entity; // Entityオブジェクト自体を解放する
	entity = nullptr;
}

void World::AddSystem(std::unique_ptr<System> sys)
{
	m_systems.push_back(std::move(sys));
}

void World::Start(World& world)
{
	m_cameraSystem = GetSystem<CameraSystem>();

	for (auto& sys : m_systems)
	{
		sys->Start(m_cm, world);
	}
}

void World::Update(World& world)
{
	for (auto& sys : m_systems)
	{
		sys->Update(m_cm, world);
	}
}

void World::Draw(World& world)
{
	for (Entity* entity : m_allCameraEntities)
	{
		m_cameraSystem->SetCurrent(world.GetComponent<Camera>(*entity), entity);
	}

	for (auto& sys : m_systems)
	{
		sys->InternalRender(m_cm, world);
		sys->Draw(m_cm, world);
	}

	m_cameraSystem->SetCurrent(nullptr, nullptr);
}