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
	return entity;
}

/// <summary>
/// 読み込み済みのメッシュとマテリアルから3Dモデルのエンティティを生成します。
/// </summary>
/// <param name="meshes">モデルを構成するメッシュの配列</param>
/// <param name="materials">モデルが使用するマテリアルの配列</param>
/// <param name="parent">親となるTransform</param>
/// <param name="localPosition">ローカル座標</param>
/// <param name="localRotation">ローカル回転</param>
/// <returns>生成されたエンティティ</returns>
Entity* World::CreateWithModel(
	const std::vector<ComPtr<Mesh>>& meshes,
	const std::vector<ComPtr<Material>>& materials,
	Transform* parent,
	const Vector3& localPosition,
	const Quaternion& localRotation)
{
	// 1. 基本的なエンティティを作成（Transformコンポーネントが自動で付与される）
	Entity* entity = CreateEntity();

	// 2. MeshRendererコンポーネントを作成し、メッシュとマテリアルを設定
	MeshRenderer renderer;
	renderer.meshes = meshes;
	renderer.materials = materials;
	AddComponent<MeshRenderer>(*entity, renderer);

	// 3. Transformを設定
	TransformSystem* transformSystem = GetSystem<TransformSystem>();
	Transform* transform = GetComponent<Transform>(*entity);
	transformSystem->SetLocalPosition(*transform, localPosition);
	transformSystem->SetLocalRotation(*transform, localRotation);
	// TODO: parentの設定処理を追加

	return entity;
}

/// <summary>
/// ファイルパスから3Dモデルを読み込み、エンティティを生成します。
/// </summary>
/// <param name="path">モデルファイルへのパス (例: "Assets/character.fbx")</param>
/// <param name="parent">親となるTransform</param>
/// <param name="localPosition">ローカル座標</param>
/// <param name="localRotation">ローカル回転</param>
/// <returns>生成されたエンティティ。読み込みに失敗した場合はnullptr</returns>
Entity* World::CreateWithModel(const std::string& path, Transform* parent, const Vector3& localPosition, const Quaternion& localRotation)
{
	// 1. 作成済みのModelImporterを使ってモデルデータを読み込む
	ModelImporter importer;
	if (!importer.Import(path))
	{
		// 読み込みに失敗した場合
		std::string err_msg = "Failed to load model: " + path;
		OutputDebugStringA(err_msg.c_str());
		return nullptr;
	}

	// 2. 読み込んだデータを使って、コアとなる生成関数を呼び出す
	return CreateWithModel(importer.meshes, importer.materials, parent, localPosition, localRotation);
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
	return entity;
}

void World::DestoryEntity(Entity entity)
{
	m_cm.RemoveAllComponents(entity);
	m_em.DestroyEntity(entity);
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
	static std::vector<Camera*> validCamera;

	validCamera.clear();
	for (Camera* camera : m_allCameras)
	{
		validCamera.push_back(camera);
	}

	for (Camera* camera : validCamera)
	{
		m_cameraSystem->SetCurrent(camera);
	}

	for (auto& sys : m_systems)
	{
		sys->InternalRender(m_cm, world);
		sys->Draw(m_cm, world);
	}

	m_cameraSystem->SetCurrent(nullptr);
}