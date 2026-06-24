#include "World.h"
#include "TransformSystem.h"
#include "ShaderRegistry.h"

void World::CollectDescendantsRecursive(Transform* parent, std::vector<Entity>& descendants)
{

}

void World::Clear()
{
	auto entityCopy = m_allEntities;
	for (Entity e : entityCopy)
	{
		if (m_em.IsAlive(e))
		{
			m_am.DestroyEntity(e);
			m_em.DestroyEntity(e);
		}
	}
	m_allEntities.clear();
	m_allRootEntities.clear();
	m_allCameraEntities.clear();
	m_allCameras.clear();
	m_entityNames.clear();
	m_systems.clear();
	m_transformSystem = nullptr;
	m_cameraSystem = nullptr;
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

Entity World::CreateWithLight(const LightType& type, const Color& color, const float range, Transform* parent, const Vector3& pos, const Quaternion& rot)
{
	Entity entity = CreateEntity();

	Light light;
	light.type = type;
	light.color = color;
	light.range = range;
	AddComponent<Light>(entity, light);

	// ライトの向きをTransformの回転で制御する
	Transform* lightTransform = GetComponent<Transform>(entity);
	TransformSystem::GetInstance()->SetParent(*lightTransform, parent);
	lightTransform->position = pos;
	lightTransform->rotation = rot;

	return Entity();
}

Entity World::CreateWithMesh(std::vector<Vector3>&& vertices, std::vector<uint32_t>&& indices, std::vector<Vector2>&& uvs, LayerMask layer, Color color, bool isWireframe, Texture2D* tex)
{
	Entity entity = CreateEntity("Generated Mesh", layer);

	// 1. UVが空の場合のフォールバック (XZ投影)
	if (uvs.empty()) {
		uvs.resize(vertices.size());
		for (size_t i = 0; i < vertices.size(); i++) {
			uvs[i] = Vector2(vertices[i].x, vertices[i].z);
		}
	}

	// 2. Normal と Tangent の計算用バッファ
	std::vector<Vector3> calculatedNormals(vertices.size(), Vector3::zero);
	std::vector<Vector3> calculatedTangents(vertices.size(), Vector3::zero);

	for (size_t i = 0; i < indices.size(); i += 3)
	{
		uint32_t i0 = indices[i], i1 = indices[i + 1], i2 = indices[i + 2];
		Vector3 edge1 = vertices[i1] - vertices[i0];
		Vector3 edge2 = vertices[i2] - vertices[i0];

		// --- Normal の計算 ---
		Vector3 faceNormal = Vector3::Cross(edge1, edge2);
		calculatedNormals[i0] += faceNormal;
		calculatedNormals[i1] += faceNormal;
		calculatedNormals[i2] += faceNormal;

		// --- Tangent の計算 ---
		Vector2 duv1 = uvs[i1] - uvs[i0];
		Vector2 duv2 = uvs[i2] - uvs[i0];
		float f = (duv1.x * duv2.y - duv2.x * duv1.y);
		f = (abs(f) > 1e-6f) ? 1.0f / f : 0.0f;

		Vector3 tangent = (edge1 * duv2.y - edge2 * duv1.y) * f;
		calculatedTangents[i0] += tangent;
		calculatedTangents[i1] += tangent;
		calculatedTangents[i2] += tangent;
	}

	Mesh* mesh = new Mesh();
	for (size_t i = 0; i < vertices.size(); i++)
	{
		Mesh::Vertex vertex;
		vertex.position = vertices[i];
		vertex.uv = uvs[i];
		vertex.normal = Vector3::Normalize(calculatedNormals[i]);

		if (calculatedTangents[i].SqrMagnitude() > 0.0001f)
			vertex.tangent = Vector3::Normalize(calculatedTangents[i]);
		else
			vertex.tangent = Vector3(1, 0, 0);

		mesh->AddVertex(vertex);
	}

	mesh->AddSubMesh(0, (UINT)indices.size(), 0);
	mesh->SetIndices(std::move(indices), 0);
	mesh->SetupMesh();

	// 以降、RendererとMaterialの設定（既存通り）
	MeshFilter* meshFilter = AddComponent<MeshFilter>(entity, {});
	meshFilter->mesh = mesh;

	std::vector<Material*> materials(1, new Material());
	materials[0]->SetMainTexture(tex ? tex : AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/white.png"));
	materials[0]->SetTexture("_NormalTex", AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, L"Assets/T_Wall_N.png"));
	materials[0]->SetBaseColor(color);
	materials[0]->SetFloat("_Metallic", 0.5f);
	materials[0]->SetFloat("_Roughness", 0.5f);
	materials[0]->SetFloat("_Occlusion", 1.0f);

	if (isWireframe) {
		WFMeshRenderer wr; wr.materials = materials; AddComponent<WFMeshRenderer>(entity, wr);
	}
	else {
		MeshRenderer mr; mr.materials = materials; AddComponent<MeshRenderer>(entity, mr);
	}
	
	Collider coll;
	coll.type = ColliderType::AABB;
	coll.size = mesh->GetBounds().GetSize();
	AddComponent<Collider>(entity, coll);

	return entity;
}

Entity World::CreateCube(float xLength, float yLength, float zLength, LayerMask layer, Color color, bool isWireframe, Texture2D* tex)
{
	const Vector3 h = Vector3(xLength * 0.5f, yLength * 0.5f, zLength * 0.5f);

	std::vector<Vector3> vertices = {
		// Front
		{-h.x, -h.y, -h.z}, {-h.x,  h.y, -h.z}, { h.x, -h.y, -h.z}, { h.x,  h.y, -h.z},
		// Right
		{ h.x, -h.y, -h.z}, { h.x,  h.y, -h.z}, { h.x, -h.y,  h.z}, { h.x,  h.y,  h.z},
		// Back
		{ h.x, -h.y,  h.z}, { h.x,  h.y,  h.z}, {-h.x, -h.y,  h.z}, {-h.x,  h.y,  h.z},
		// Left
		{-h.x, -h.y,  h.z}, {-h.x,  h.y,  h.z}, {-h.x, -h.y, -h.z}, {-h.x,  h.y, -h.z},
		// Top
		{-h.x,  h.y, -h.z}, {-h.x,  h.y,  h.z}, { h.x,  h.y, -h.z}, { h.x,  h.y,  h.z},
		// Bottom
		{-h.x, -h.y,  h.z}, {-h.x, -h.y, -h.z}, { h.x, -h.y,  h.z}, { h.x, -h.y, -h.z}
	};

	std::vector<Vector2> uvs;
	// 全ての面に 0~1 のUVを割り当て
	for (int i = 0; i < 6; ++i) {
		uvs.push_back({ 0, 1 }); uvs.push_back({ 0, 0 }); uvs.push_back({ 1, 1 }); uvs.push_back({ 1, 0 });
	}

	std::vector<uint32_t> indices;
	for (uint32_t i = 0; i < 6; ++i) {
		uint32_t offset = i * 4;
		indices.push_back(offset + 0); indices.push_back(offset + 1); indices.push_back(offset + 2);
		indices.push_back(offset + 2); indices.push_back(offset + 1); indices.push_back(offset + 3);
	}

	return CreateWithMesh(std::move(vertices), std::move(indices), std::move(uvs), layer, color, isWireframe, tex);
}

Entity World::CreateSphere(float radius, uint16_t slices, uint16_t stacks, LayerMask layer, Color color, bool isWireframe, Texture2D* tex)
{
	std::vector<Vector3> vertices;
	std::vector<Vector2> uvs;

	for (int j = 0; j <= stacks; j++) 
	{
		float phi = Mathf::PI * (float)j / stacks;
		float y = radius * cosf(phi);
		float r = radius * sinf(phi);

		for (int i = 0; i <= slices; i++) 
		{
			float theta = 2.0f * Mathf::PI * (float)i / slices;
			float x = r * cosf(theta);
			float z = r * sinf(theta);

			vertices.emplace_back(x, y, z);
			// テクスチャが裏返る場合は、(1.0f - (float)i / slices) のように反転させます
			uvs.emplace_back(1.0f - (float)i / slices, (float)j / stacks);
		}
	}

	std::vector<uint32_t> indices;
	for (int j = 0; j < stacks; j++) 
	{
		for (int i = 0; i < slices; i++) 
		{
			uint32_t first = (j * (slices + 1)) + i;
			uint32_t second = first + slices + 1;

			// インデックスの順序を [first, second, first + 1] から入れ替えて
			// 面が外側を向くように調整します
			indices.push_back(first);
			indices.push_back(first + 1);
			indices.push_back(second);

			indices.push_back(second);
			indices.push_back(first + 1);
			indices.push_back(second + 1);
		}
	}

	return CreateWithMesh(std::move(vertices), std::move(indices), std::move(uvs), layer, color, isWireframe, tex);
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
	SpriteAPI::SetSprite(renderer, sprite);
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

Entity World::CreateCamera3D(float fieldOfView, float aspect, float nearClipPlane, float farClipPlane, const Color& color, const Vector3& localPosition, const Quaternion& localRotation)
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
	camera->backgroundColor = color;		// 背景色
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

	// TransformSystemに通知して階層関係をクリーンアップ（子の再帰削除もここで行われる）
	if (m_transformSystem)
	{
		m_transformSystem->OnEntityDestroyed(*this, entity);
	}

	m_entityNames.erase(entity.name);
	m_am.DestroyEntity(entity);
	m_em.DestroyEntity(entity);
	m_allEntities.remove(entity);
}

void World::AddSystem(std::unique_ptr<System> sys)
{
	m_systems.push_back(std::move(sys));
}

bool World::Load(World& world)
{
	world.AddSystem(std::make_unique<CameraSystem>());
	world.AddSystem(std::make_unique<LightSystem>());
	world.AddSystem(std::make_unique<SpriteRendererSystem>());
	world.AddSystem(std::make_unique<MeshRendererSystem>());
	world.AddSystem(std::make_unique<AnimationSystem>());
	world.AddSystem(std::make_unique<BoneSocketSystem>());
	world.AddSystem(std::make_unique<TransformSystem>());
	world.AddSystem(std::make_unique<CollisionSystem>());
	world.AddSystem(std::make_unique<SkinnedMeshRendererSystem>());
	world.AddSystem(std::make_unique<InputSystem>());
	world.AddSystem(std::make_unique<ProjectileSystem>());
	world.AddSystem(std::make_unique<AIAgentSystem>());
	world.AddSystem(std::make_unique<HPSystem>());
	world.AddSystem(std::make_unique<PhysicsSystem>());
	world.AddSystem(std::make_unique<UILayoutSystem>());
	world.AddSystem(std::make_unique<UICanvasSystem>());
	world.AddSystem(std::make_unique<UIEventSystem>());
	world.AddSystem(std::make_unique<UIButtonSystem>());
	world.AddSystem(std::make_unique<UISliderSystem>());
	world.AddSystem(std::make_unique<AudioSystem>());

	TransformSystem::CreateSingleton();
	AIAgentSystem::CreateSingleton();
	UIManager::CreateSingleton();
	m_transformSystem = TransformSystem::GetInstance();

	AssetManager::GetInstance()->LoadAsset(AssetType::Texture, L"Assets/White.png");
	AssetManager::GetInstance()->LoadAsset(AssetType::Texture, L"Assets/DefaultNormalMap.jpg");
	AssetManager::GetInstance()->LoadAsset(AssetType::Texture, L"Assets/DefaultMetallicRoughnessMap.jpg");
	AssetManager::GetInstance()->LoadAsset(AssetType::Texture, L"Assets/DefaultAOMap.jpg");

	for (auto& sys : m_systems)
	{
		if (!sys->Load(world))
		{
			return false;
		}
	}
	return true;
}

bool World::Unload(World& world)
{
	TransformSystem::DestroySingleton();
	AIAgentSystem::DestroySingleton();
	UIManager::DestroySingleton();

	m_allCameraEntities.clear();
	m_allCameras.clear();
	m_allEntities.clear();
	m_allRootEntities.clear();
	m_entityNames.clear();
	m_systems.clear();
	m_transformSystem = nullptr;
	m_cameraSystem = nullptr;

	m_am.Clear();
	m_em.Clear();

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