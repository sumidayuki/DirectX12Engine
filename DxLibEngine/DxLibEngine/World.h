#pragma once
#include "SystemList.h"

/// <summary>
/// �N���X���FWorld
/// �T�v�FECS�̑S�̂��Ǘ�����N���X�ł��B
/// �p�r�F�G���e�B�e�B�̐����E�j��A�R���|�[�l���g�̑���A�V�X�e���̊Ǘ����s���܂��B
/// </summary>
class World
{
private:
	EntityManager							m_em;									
	ComponentManager						m_cm;
	std::vector<std::unique_ptr<System>>	m_systems;
	CameraSystem*							m_cameraSystem;
	std::list<Camera*>						m_allCameras;

public:
	/// <summary>
	/// �G���e�B�e�B���쐬���܂��B
	/// </summary>
	/// <returns>Entity�^�Œl��Ԃ��܂��B</returns>
	Entity* CreateEntity();

	/// <summary>
	/// ComponentManager�ւ̎Q�Ƃ��擾���܂��B
	/// </summary>
	/// <returns>ComponentManager�̎Q��</returns>
	ComponentManager& GetComponentManager() { return m_cm; }

	Entity* CreateWithSprite

	(
		const wchar_t* path,
		const Rect& rect,
		const Vector2 pivot = Vector2::zero,
		float pixelsPerUnit = 100.0f,
		Transform* parent = nullptr,
		const Vector3& localPosition = Vector3::zero,
		const Quaternion& localRotation = Quaternion::identity
	);

	Entity* CreateWithSprite
	(
		Texture2D* texture,
		const Rect& rect,
		const Vector2 pivot = Vector2::zero,
		float pixelsPerUnit = 100.0f,
		Transform* parent = nullptr,
		const Vector3& localPosition = Vector3::zero,
		const Quaternion& localRotation = Quaternion::identity
	);

	Entity* CreateWithSprite
	(
		Sprite* sprite,
		Transform* parent = nullptr,
		const Vector3& localPosition = Vector3::zero,
		const Quaternion& localRotation = Quaternion::identity
	);

	Entity* CreateCamera2D(float viewWidth, float viewHeight, const Vector3& localPosition = Vector3::zero, const Quaternion& localRotation = Quaternion::identity);

	Entity* CreateWithModel
	(
		const std::string& path,
		Transform* parent,
		const Vector3& localPosition,
		const Quaternion& localRotation
	);

	Entity* CreateWithModel
	(
		const std::vector<ComPtr<Mesh>>& meshes,
		const std::vector<ComPtr<Material>>& materials,
		Transform* parent,
		const Vector3& localPosition,
		const Quaternion& localRotation
	);

	Entity* CreateCamera3D(float fieldOfView, float aspect, float nearClipPlane, float farClipPlane, const Vector3& localPosition = Vector3::zero, const Quaternion& localRotation = Quaternion::identity);


	/// <summary>
	/// �G���e�B�e�B��j�󂵂܂��B
	/// </summary>
	/// <param name="entity">�j�󂵂���Entity</param>
	void DestoryEntity(Entity entity);

	/// <summary>
	/// �w�肵���G���e�B�e�B���������Ă��邩���m�F���܂��B
	/// </summary>
	/// <param name="entity">�m�F������Entity</param>
	/// <returns>�������Ă���ꍇ�� true�A�������Ă��Ȃ��ꍇ�� false ��Ԃ��܂��B</returns>
	bool IsAlive(Entity entity)const { return m_em.IsAlive(entity); }

	/// <summary>
	/// ����̃G���e�B�e�B�Ɏw�肵���R���|�[�l���g��ǉ����܂��B
	/// </summary>
	/// <typeparam name="T">�ǉ��������R���|�[�l���g���w�肵�܂��B</typeparam>
	/// <param name="e">�R���|�[�l���g��ǉ�����G���e�B�e�B</param>
	/// <param name="c">�ǉ�����R���|�[�l���g�̏�����Ԃ�ݒ肵�܂��B</param>
	template <typename T> void AddComponent(Entity e, const T& c) { m_cm.AddComponent(e, c); }

	/// <summary>
	/// ����̃G���e�B�e�B����w�肵���R���|�[�l���g���擾���܂��B
	/// </summary>
	/// <typeparam name="T">�擾�������R���|�[�l���g���w�肵�܂��B</typeparam>
	/// <param name="e">�R���|�[�l���g�擾��̃G���e�B�e�B</param>
	/// <returns>�擾�����R���|�[�l���g���|�C���^�^�ŕԂ��܂��B</returns>
	template <typename T> T* GetComponent(Entity e) { return m_cm.GetComponent<T>(e); }

	/// <summary>
	/// ����̃G���e�B�e�B���w�肵���R���|�[�l���g���������Ă��邩�m�F���܂��B
	/// </summary>
	/// <typeparam name="T">�m�F�������R���|�[�l���g���w�肵�܂��B</typeparam>
	/// <param name="e">�m�F�������G���e�B�e�B</param>
	/// <returns>�������Ă����� true �������Ă��Ȃ���� false ��Ԃ��܂��B</returns>
	template <typename T> bool HasComponent(Entity e) { return m_cm.HasComponent<T>(e); }

	/// <summary>
	/// �w�肵���V�X�e����ǉ����܂��B
	/// </summary>
	/// <param name="sys">make_unique<�ǉ�������System>�Ŏw�肵�܂��B</param>
	void AddSystem(std::unique_ptr<System> sys);

	/// <summary>
	/// �w�肵���V�X�e�����擾���܂��B
	/// </summary>
	/// <typeparam name="T">�擾������System���w�肵�܂��B�iSystem�N���X���p�����Ă�����̂����ł��B�j</typeparam>
	/// <returns>�擾�����V�X�e�����|�C���^�^�ŕԂ��܂��B</returns>
	template <typename T>
	T* GetSystem()
	{
		for (auto& sys : m_systems) 
		{
			if (auto ptr = dynamic_cast<T*>(sys.get())) 
			{
				return ptr;
			}
		}
		throw std::runtime_error("System not found");
	}

	void Start(World& world);

	/// <summary>
	/// ���[���h�ɂ��邷�ׂĂ�Update���Ǘ����܂��B
	/// ���b��60��X�V����ADraw������ɏ������s���܂��B
	/// </summary>
	void Update(World& world);

	/// <summary>
	/// ���[���h�ɂ���S�Ă�Draw���Ǘ����܂��B
	/// ���b��60��X�V����AUpdate������ɏ������s���܂��B
	/// </summary>
	/// <param name="world"></param>
	void Draw(World& world);
};