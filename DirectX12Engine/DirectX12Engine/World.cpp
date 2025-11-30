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

Entity World::CreateEntity(const std::string& name, LayerMask layer)
{
	// Entity を entityManager を使用してエンティティを生成します。
	Entity entity = m_em.CreateEntity();

	std::string finalName = name;
	int index = 1;
	// m_entityNames に同じ名前が存在する限り、サフィックスの数字を増やして新しい名前を試す
	//while (m_entityNames.count(finalName))
	//{
	//	finalName = name + "_" + std::to_string(index++);
	//}
	
	// ユニークな名前をエンティティに設定し、セットに追加
	entity.name = finalName;
	m_entityNames.insert(finalName);

	const Archetype* emptyArchetype = m_am.GetOrCreateArchetype({});
	m_am.AddEntity(emptyArchetype, entity);

	Transform transform;
	transform.entity = entity;

	// Entity に Transform コンポーネントを付与しておきます。
	AddComponent<Transform>(entity, transform);

	// Entity に Layer コンポーネントを付与しておきます。
	Layer layerComp;
	layerComp.layer = layer;
	AddComponent<Layer>(entity, layerComp);

	m_allEntities.push_back(entity);

	return entity;
}

Entity World::CreateWithMesh(std::vector<Vector3>&& vertices, std::vector<uint32_t>&& indices, LayerMask layer, Color color, bool isWireframe)
{
	Entity entity = CreateEntity("空のエンティティ", layer);

	// メッシュを包含する最小のAABBを計算します。
	Vector3 min = vertices[0];
	Vector3 max = vertices[0];
	for (size_t i = 1; i < vertices.size(); i++)
	{
		min = Vector3::Min(vertices[i], min);
		max = Vector3::Max(vertices[i], max);
	}
	
	Mesh* mesh = new Mesh();
	
	for (int i = 0; i < vertices.size(); i++)
	{
		Mesh::Vertex vertex;
		vertex.position = vertices[i];
		vertex.normal = Vector3::up; // 仮の法線
		vertex.uv = Vector2::zero;    // 仮のUV
		mesh->AddVertex(vertex);
	}
	mesh->AddSubMesh(0, (UINT)indices.size(), 0);
	mesh->SetIndices(std::move(indices), 0);
	mesh->SetupMesh();

	MeshFilter* meshFilter = AddComponent<MeshFilter>(entity, {});
	meshFilter->mesh = mesh;

	// マテリアル配列の作成
	std::vector<Material*> materials;
	materials.resize(1);
	materials[0] = new Material();
	materials[0]->SetTexture(Material::TextureSlot::Diffuse, AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/white.png"));
	materials[0]->SetDiffuseColor(color);
	materials[0]->SetSpecularColor(Color::white);

	if (isWireframe)
	{
		WFMeshRenderer* wfMeshRenderer = AddComponent<WFMeshRenderer>(entity, {});
		wfMeshRenderer->materials = materials;
	}
	else
	{
		MeshRenderer* meshRenderer = AddComponent<MeshRenderer>(entity, {});
		meshRenderer->materials = materials;
	}

	return entity;
}

Entity World::CreateCube(float xLength, float yLength, float zLength, LayerMask layer, Color color, bool isWireframe)
{
	// 各辺の半分の長さを計算する
	const Vector3 halfExtents = Vector3(xLength / 2, yLength / 2, zLength / 2);

	// 頂点配列を作成する
	std::vector<Vector3> vertices =
	{
		{ -halfExtents.x, -halfExtents.y, -halfExtents.z },   // 前面の左下
		{ -halfExtents.x, +halfExtents.y, -halfExtents.z },   // 前面の左上
		{ +halfExtents.x, -halfExtents.y, -halfExtents.z },   // 前面の右下
		{ +halfExtents.x, +halfExtents.y, -halfExtents.z },   // 前面の右上

		{ +halfExtents.x, -halfExtents.y, -halfExtents.z },   // 右面の左下
		{ +halfExtents.x, +halfExtents.y, -halfExtents.z },   // 右面の左上
		{ +halfExtents.x, -halfExtents.y, +halfExtents.z },   // 右面の右下
		{ +halfExtents.x, +halfExtents.y, +halfExtents.z },   // 右面の右上

		{ +halfExtents.x, -halfExtents.y, +halfExtents.z },   // 背面の左下
		{ +halfExtents.x, +halfExtents.y, +halfExtents.z },   // 背面の左上
		{ -halfExtents.x, -halfExtents.y, +halfExtents.z },   // 背面の右下
		{ -halfExtents.x, +halfExtents.y, +halfExtents.z },   // 背面の右上

		{ -halfExtents.x, -halfExtents.y, +halfExtents.z },   // 左面の左下
		{ -halfExtents.x, +halfExtents.y, +halfExtents.z },   // 左面の左上
		{ -halfExtents.x, -halfExtents.y, -halfExtents.z },   // 左面の右下
		{ -halfExtents.x, +halfExtents.y, -halfExtents.z },   // 左面の右上

		{ -halfExtents.x, +halfExtents.y, -halfExtents.z },   // 上面の左下
		{ -halfExtents.x, +halfExtents.y, +halfExtents.z },   // 上面の左上
		{ +halfExtents.x, +halfExtents.y, -halfExtents.z },   // 上面の右下
		{ +halfExtents.x, +halfExtents.y, +halfExtents.z },   // 上面の右上

		{ -halfExtents.x, -halfExtents.y, +halfExtents.z },   // 底面の左下
		{ -halfExtents.x, -halfExtents.y, -halfExtents.z },   // 底面の左上
		{ +halfExtents.x, -halfExtents.y, +halfExtents.z },   // 底面の右下
		{ +halfExtents.x, -halfExtents.y, -halfExtents.z },   // 底面の右上
	};

	// インデックス配列を作成する
	std::vector<uint32_t> indices =
	{
		 0,  1,  2,
		 2,  1,  3,
		 4,  5,  6,
		 6,  5,  7,
		 8,  9, 10,
		10,  9, 11,
		12, 13, 14,
		14, 13, 15,
		16, 17, 18,
		18, 17, 19,
		20, 21, 22,
		22, 21, 23,
	};

	return CreateWithMesh(std::move(vertices), std::move(indices), layer, color, isWireframe);
}

Entity World::CreateSphere(float radius, uint16_t slices, uint16_t stacks, LayerMask layer, Color color, bool isWireframe)
{
	// 頂点数を計算する
	const int vertexCount = (slices + 1) * (stacks + 1);

	// 頂点配列を作成する
	std::vector<Vector3> vertices;

	// 1周分の角度θの増分を計算する
	const float deltaTheta = 360.0f / slices;

	// 半周分の角度φの増分を計算する
	const float deltaPhi = 180.0f / stacks;

	for (int j = 0; j < stacks + 1; j++)
	{
		// 角度φ (-90°～ +90°)
		const float phi = -90 + deltaPhi * j;
		const float y = radius * Mathf::Sin(phi * Mathf::Deg2Rad);
		const float r = radius * Mathf::Cos(phi * Mathf::Deg2Rad);

		for (int i = 0; i < slices + 1; i++)
		{
			// 角度θ (0°～360°)
			const float theta = deltaTheta * i;
			const float x = r * Mathf::Cos(theta * Mathf::Deg2Rad);
			const float z = r * Mathf::Sin(theta * Mathf::Deg2Rad);
			vertices.emplace_back(x, y, z);
		}
	}

	// インデックス数を計算する。
	const int indexCount = slices * stacks * 6;

	// インデックス配列を作成する
	std::vector<uint32_t> indices;
	indices.reserve(indexCount);

	for (int j = 0; j < stacks; j++)
	{
		for (int i = 0; i < slices; i++)
		{
			const int first = (j * (slices + 1)) + i;
			const int second = first + slices + 1;
			// 左下三角形
			indices.push_back(first);
			indices.push_back(second);
			indices.push_back(first + 1);
			indices.push_back(first + 1);
			indices.push_back(second);
			indices.push_back(second + 1);
		}
	}

	return CreateWithMesh(std::move(vertices), std::move(indices), layer, color, isWireframe);
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

Entity World::CreateWithModel(const std::wstring& path, Transform* parent, const Vector3& localPosition, const Quaternion& localRotation, LayerMask layer)
{
	return AssetManager::GetInstance()->Instantiate(path, parent, localPosition, localRotation, layer);
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

	if (!IsAlive(entity)) return;

	std::vector<Entity> fullDelectionList;
	Transform* rootTransform = GetComponent<Transform>(entity);
	CollectDescendantsRecursive(rootTransform, fullDelectionList);
	fullDelectionList.push_back(entity);

	for (Entity e : fullDelectionList)
	{
		if (m_em.IsAlive(e))
		{
			Transform* t = GetComponent<Transform>(e);

			m_transformSystem->SetParent(*t, nullptr);

			m_entityNames.erase(e.name);
			m_am.DestroyEntity(e);
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
	world.AddSystem(std::make_unique<WFMeshRendererSystem>());
	world.AddSystem(std::make_unique<AnimationSystem>());
	world.AddSystem(std::make_unique<TransformSystem>());
	world.AddSystem(std::make_unique<CollisionSystem>());
	world.AddSystem(std::make_unique<SkinnedMeshRendererSystem>());
	world.AddSystem(std::make_unique<LightSystem>());
	world.AddSystem(std::make_unique<InputSystem>());
	world.AddSystem(std::make_unique<ProjectileSystem>());
	world.AddSystem(std::make_unique<AIAgentSystem>());

	TransformSystem::CreateSingleton();
	AIAgentSystem::CreateSingleton();
	m_transformSystem = TransformSystem::GetInstance();

	AssetManager::GetInstance()->LoadAsset(AssetType::Texture, L"Assets/White.png");

	for (auto& sys : m_systems)
	{
		if (!sys->Load(world))
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
		sys->Start(world);
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
		sys->Update(world);
	}
}

void World::Draw(World& world)
{
	m_cameraSystem->SetCurrent(nullptr, INVALID_ENTITY);

	for (Entity entity : m_allCameraEntities)
	{
		m_cameraSystem->SetCurrent(world.GetComponent<Camera>(entity), entity);
	}

	for (auto& sys : m_systems)
	{
		sys->InternalRender(world);
		sys->Draw(world);
	}
}