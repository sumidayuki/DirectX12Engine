#include "World.h"

Entity* World::CreateEntity()
{
	// Entity �� entityManager ���g�p���ăG���e�B�e�B�𐶐����܂��B
	Entity* entity = m_em.CreateEntity();

	// Entity �� Transform �R���|�[�l���g��t���Ă����܂��B
	m_cm.AddComponent<Transform>(*entity, Transform{});

	return entity;
}

Entity* World::CreateWithSprite(const wchar_t* path, const Rect& rect, const Vector2 pivot, float pixelsPerUnit, Transform* parent, const Vector3& localPosition, const Quaternion& localRotation)
{
	TextureImporter importer;

	// �e�N�X�`���̃��[�h
	ComPtr<Texture2D> texture;
	texture.Attach(importer.Import(path));

	return CreateWithSprite(texture.Get(), rect, pivot, pixelsPerUnit, parent, localPosition, localRotation);
}

Entity* World::CreateWithSprite(Texture2D* texture, const Rect& rect, const Vector2 pivot, float pixelsPerUnit, Transform* parent, const Vector3& localPosition, const Quaternion& localRotation)
{
	// �X�v���C�g�̍쐬
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
	// 2D�����J�����̍쐬
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
/// �ǂݍ��ݍς݂̃��b�V���ƃ}�e���A������3D���f���̃G���e�B�e�B�𐶐����܂��B
/// </summary>
/// <param name="meshes">���f�����\�����郁�b�V���̔z��</param>
/// <param name="materials">���f�����g�p����}�e���A���̔z��</param>
/// <param name="parent">�e�ƂȂ�Transform</param>
/// <param name="localPosition">���[�J�����W</param>
/// <param name="localRotation">���[�J����]</param>
/// <returns>�������ꂽ�G���e�B�e�B</returns>
Entity* World::CreateWithModel(
	const std::vector<ComPtr<Mesh>>& meshes,
	const std::vector<ComPtr<Material>>& materials,
	Transform* parent,
	const Vector3& localPosition,
	const Quaternion& localRotation)
{
	// 1. ��{�I�ȃG���e�B�e�B���쐬�iTransform�R���|�[�l���g�������ŕt�^�����j
	Entity* entity = CreateEntity();

	// 2. MeshRenderer�R���|�[�l���g���쐬���A���b�V���ƃ}�e���A����ݒ�
	MeshRenderer renderer;
	renderer.meshes = meshes;
	renderer.materials = materials;
	AddComponent<MeshRenderer>(*entity, renderer);

	// 3. Transform��ݒ�
	TransformSystem* transformSystem = GetSystem<TransformSystem>();
	Transform* transform = GetComponent<Transform>(*entity);
	transformSystem->SetLocalPosition(*transform, localPosition);
	transformSystem->SetLocalRotation(*transform, localRotation);
	// TODO: parent�̐ݒ菈����ǉ�

	return entity;
}

/// <summary>
/// �t�@�C���p�X����3D���f����ǂݍ��݁A�G���e�B�e�B�𐶐����܂��B
/// </summary>
/// <param name="path">���f���t�@�C���ւ̃p�X (��: "Assets/character.fbx")</param>
/// <param name="parent">�e�ƂȂ�Transform</param>
/// <param name="localPosition">���[�J�����W</param>
/// <param name="localRotation">���[�J����]</param>
/// <returns>�������ꂽ�G���e�B�e�B�B�ǂݍ��݂Ɏ��s�����ꍇ��nullptr</returns>
Entity* World::CreateWithModel(const std::string& path, Transform* parent, const Vector3& localPosition, const Quaternion& localRotation)
{
	// 1. �쐬�ς݂�ModelImporter���g���ă��f���f�[�^��ǂݍ���
	ModelImporter importer;
	if (!importer.Import(path))
	{
		// �ǂݍ��݂Ɏ��s�����ꍇ
		std::string err_msg = "Failed to load model: " + path;
		OutputDebugStringA(err_msg.c_str());
		return nullptr;
	}

	// 2. �ǂݍ��񂾃f�[�^���g���āA�R�A�ƂȂ鐶���֐����Ăяo��
	return CreateWithModel(importer.meshes, importer.materials, parent, localPosition, localRotation);
}

Entity* World::CreateCamera3D(float fieldOfView, float aspect, float nearClipPlane, float farClipPlane, const Vector3& localPosition, const Quaternion& localRotation)
{
	// 3D�����J�����̍쐬
	Entity* entity = CreateEntity();
	AddComponent<Camera>(*entity, Camera{});
	Camera* camera = GetComponent<Camera>(*entity);

	camera->orthographic = false;							// 3D�Ȃ̂� orthographic �� false �ɂ���
	camera->fieldOfView = fieldOfView;						// ����p
	camera->aspect = aspect;								// �A�X�y�N�g��
	camera->nearClipPlane = nearClipPlane;					// �ߕ���
	camera->farClipPlane = farClipPlane;					// ������
	camera->clearFlags = CameraClearFlags::SolidColor;		// �w�i�̃N���A���@
	camera->backgroundColor = Color::cornflowerBlue;		// �w�i�F
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