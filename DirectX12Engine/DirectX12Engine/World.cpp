#include "World.h"

void World::CollectDescendantsRecursive(Transform* parent, std::vector<Entity>& descendants)
{
	if (!parent) return;

	auto childCopy = parent->children;
	for (auto child : childCopy)
	{
		Transform* childTransform = GetComponent<Transform>(child);

		if (childTransform && GetComponent<Transform>(childTransform->entity))
		{
			descendants.push_back(childTransform->entity);
			CollectDescendantsRecursive(childTransform, descendants);
		}
	}
}

Entity World::CreateEntity(const std::string& name)
{
	// Entity を entityManager を使用してエンティティを生成します。
	Entity entity = m_em.CreateEntity();

	std::string finalName = name;
	int index = 1;
	// m_entityNames に同じ名前が存在する限り、サフィックスの数字を増やして新しい名前を試す
	while (m_entityNames.count(finalName))
	{
		finalName = name + "_" + std::to_string(index++);
	}

	OutputDebugStringA((finalName + "\n").c_str());

	// ユニークな名前をエンティティに設定し、セットに追加
	entity.name = finalName;
	m_entityNames.insert(finalName);

	Transform transform;
	transform.entity = entity;

	// Entity に Transform コンポーネントを付与しておきます。
	m_cm.AddComponent<Transform>(entity, transform);

	m_allEntities.push_back(entity);

	return entity;
}

Entity World::CreateWithSprite(const wchar_t* path, const Rect& rect, const Vector2 pivot, float pixelsPerUnit, Transform* parent, const Vector3& localPosition, const Quaternion& localRotation)
{
	TextureImporter importer;

	// テクスチャのロード
	ComPtr<Texture2D> texture;
	texture.Attach(importer.Import(path));

	return CreateWithSprite(texture.Get(), rect, pivot, pixelsPerUnit, parent, localPosition, localRotation);
}

Entity World::CreateWithSprite(Texture2D* texture, const Rect& rect, const Vector2 pivot, float pixelsPerUnit, Transform* parent, const Vector3& localPosition, const Quaternion& localRotation)
{
	// スプライトの作成
	ComPtr<Sprite> sprite;
	sprite.Attach(Sprite::Create(texture, rect, pivot, pixelsPerUnit, pixelsPerUnit));

	return CreateWithSprite(sprite.Get(), parent, localPosition, localRotation);
}

Entity World::CreateWithSprite(Sprite* sprite, Transform* parent, const Vector3& localPosition, const Quaternion& localRotation)
{
	Entity entity = CreateEntity();

	SpriteRenderer* renderer = new SpriteRenderer();
	GetSystem<SpriteRendererSystem>()->SetSprite(renderer, sprite);
	AddComponent<SpriteRenderer>(entity, *renderer);

	TransformSystem* transformSystem = GetSystem<TransformSystem>();
	Transform* transform = GetComponent<Transform>(entity);
	transformSystem->SetLocalPosition(*transform, localPosition);
	transformSystem->SetLocalRotation(*transform, localRotation);

	return entity;
}

Entity World::CreateCamera2D(float viewWidth, float viewHeight, const Vector3& localPosition, const Quaternion& localRotation)
{
	// 2D向けカメラの作成
	Entity entity = CreateEntity();
	AddComponent<Camera>(entity, Camera{});
	Camera* camera = GetComponent<Camera>(entity);
	camera->orthographic = true;
	camera->orthographicSize = viewHeight * 0.5f;
	camera->clearFlags = CameraClearFlags::SolidColor;
	camera->backgroundColor = Color::cornflowerBlue;
	camera->aspect = viewWidth / viewHeight;
	camera->nearClipPlane = 0.3f;
	camera->farClipPlane = 1000.0f;
	camera->viewportRect = Rect(0.0f, 0.0f, 1.0f, 1.0f);

	TransformSystem* transformSystem = GetSystem<TransformSystem>();
	Transform* transform = GetComponent<Transform>(entity);
	transformSystem->SetLocalPosition(*transform, localPosition);
	transformSystem->SetLocalRotation(*transform, localRotation);

	m_allCameras.push_back(camera);
	m_allCameraEntities.push_back(entity);
	return entity;
}

Entity World::CreateWithModel(const std::wstring& path, Transform* parent, const Vector3& localPosition, const Quaternion& localRotation)
{
	return AssetManager::GetInstance()->Instantiate(path, parent, localPosition, localRotation);
}

Entity World::CreateCamera3D(float fieldOfView, float aspect, float nearClipPlane, float farClipPlane, const Vector3& localPosition, const Quaternion& localRotation)
{
	// 3D向けカメラの作成
	Entity entity = CreateEntity();
	AddComponent<Camera>(entity, Camera{});
	Camera* camera = GetComponent<Camera>(entity);

	camera->orthographic = false;							// 3Dなので orthographic は false にする
	camera->fieldOfView = fieldOfView;						// 視野角
	camera->aspect = aspect;								// アスペクト比
	camera->nearClipPlane = nearClipPlane;					// 近平面
	camera->farClipPlane = farClipPlane;					// 遠平面
	camera->clearFlags = CameraClearFlags::SolidColor;		// 背景のクリア方法
	camera->backgroundColor = Color::cornflowerBlue;		// 背景色
	camera->viewportRect = Rect(0.0f, 0.0f, 1.0f, 1.0f);

	TransformSystem* transformSystem = GetSystem<TransformSystem>();
	Transform* transform = GetComponent<Transform>(entity);
	transformSystem->SetLocalPosition(*transform, localPosition);
	transformSystem->SetLocalRotation(*transform, localRotation);

	m_allCameras.push_back(camera);
	m_allCameraEntities.push_back(entity);

	return entity;
}

void World::DestroyEntity(Entity entity)
{
	if (entity == INVALID_ENTITY || !m_em.IsAlive(entity)) return;

	std::vector<Entity> fullDelectionList;
	Transform* rootTransform = GetComponent<Transform>(entity);
	CollectDescendantsRecursive(rootTransform, fullDelectionList);
	fullDelectionList.push_back(entity);

	for (Entity e : fullDelectionList)
	{
		if (m_em.IsAlive(e))
		{
			Transform* t = GetComponent<Transform>(e);

			TransformSystem::SetParent(*t, nullptr);

			m_entityNames.erase(e.name);
			m_cm.RemoveAllComponents(e);
			m_em.DestroyEntity(e);
			m_allEntities.remove(e);
		}
	}
}

void World::AddSystem(std::unique_ptr<System> sys)
{
	m_systems.push_back(std::move(sys));
}

bool World::Load(World& world)
{
	world.AddSystem(std::make_unique<CameraSystem>());
	world.AddSystem(std::make_unique<SpriteRendererSystem>());
	world.AddSystem(std::make_unique<MeshRendererSystem>());
	world.AddSystem(std::make_unique<SkinnedMeshRendererSystem>());
	world.AddSystem(std::make_unique<TransformSystem>());
	world.AddSystem(std::make_unique<AnimationSystem>());
	world.AddSystem(std::make_unique<LightSystem>());
	world.AddSystem(std::make_unique<InputSystem>());
	world.AddSystem(std::make_unique<ProjectileSystem>());

	for (auto& sys : m_systems)
	{
		if (!sys->Load(m_cm, world))
		{
			return false;
		}
	}
	return true;
}

void World::Start(World& world)
{
	m_cameraSystem = GetSystem<CameraSystem>();

	for (auto& sys : m_systems)
	{
		sys->Start(m_cm, world);
	}
}

void World::BeginFrame(UINT frameIndex)
{
	m_srvAllocator->BeginFrame(frameIndex);
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
	for (Entity entity : m_allCameraEntities)
	{
		m_cameraSystem->SetCurrent(world.GetComponent<Camera>(entity), entity);
	}

	for (auto& sys : m_systems)
	{
		sys->InternalRender(m_cm, world);
		sys->Draw(m_cm, world);
	}

	m_cameraSystem->SetCurrent(nullptr, INVALID_ENTITY);
}