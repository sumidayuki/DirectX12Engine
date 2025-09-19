#pragma once
#include "SystemList.h"

/// <summary>
/// クラス名：World
/// 概要：ECSの全体を管理するクラスです。
/// 用途：エンティティの生成・破壊、コンポーネントの操作、システムの管理を行います。
/// </summary>
class World
{
private:
	EntityManager							m_em;									
	ComponentManager						m_cm;
	std::vector<std::unique_ptr<System>>	m_systems;
	std::unique_ptr<DescriptorAllocator>	m_srvAllocator;
	CameraSystem*							m_cameraSystem;
	std::list<Entity*>						m_allEntities;
	std::list<Entity*>						m_allCameraEntities;
	std::list<Camera*>						m_allCameras;

public:
	World() { m_srvAllocator = std::make_unique<DescriptorAllocator>(1024); }

	DescriptorAllocator* GetSrvAllocator() { return m_srvAllocator.get(); }

	/// <summary>
	/// エンティティを作成します。
	/// </summary>
	/// <returns>Entity型で値を返します。</returns>
	Entity* CreateEntity(const std::string& name = "空のエンティティ");

	/// <summary>
	/// ComponentManagerへの参照を取得します。
	/// </summary>
	/// <returns>ComponentManagerの参照</returns>
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

	/// <summary>
	/// ファイルパスから3Dモデルを読み込み、エンティティを生成します。
	/// </summary>
	/// <param name="path">モデルファイルへのパス (例: "Assets/character.fbx")</param>
	/// <param name="parent">親となるTransform</param>
	/// <param name="localPosition">ローカル座標</param>
	/// <param name="localRotation">ローカル回転</param>
	/// <returns>生成されたエンティティ。読み込みに失敗した場合はnullptr</returns>
	Entity* CreateWithModel
	(
		const std::wstring& path,
		Transform* parent,
		const Vector3& localPosition,
		const Quaternion& localRotation
	);

	Entity* CreateCamera3D(float fieldOfView, float aspect, float nearClipPlane, float farClipPlane, const Vector3& localPosition = Vector3::zero, const Quaternion& localRotation = Quaternion::identity);


	/// <summary>
	/// エンティティを破壊します。
	/// </summary>
	/// <param name="entity">破壊したいEntity</param>
	void DestroyEntity(Entity* entity);

	/// <summary>
	/// 指定したエンティティが生存しているかを確認します。
	/// </summary>
	/// <param name="entity">確認したいEntity</param>
	/// <returns>生存している場合は true、生存していない場合は false を返します。</returns>
	bool IsAlive(Entity* entity)const { return m_em.IsAlive(entity); }

	/// <summary>
	/// 特定のエンティティに指定したコンポーネントを追加します。
	/// </summary>
	/// <typeparam name="T">追加したいコンポーネントを指定します。</typeparam>
	/// <param name="e">コンポーネントを追加するエンティティ</param>
	/// <param name="c">追加するコンポーネントの初期状態を設定します。</param>
	template <typename T> void AddComponent(Entity e, const T& c) { m_cm.AddComponent(e, c); }

	/// <summary>
	/// 特定のエンティティから指定したコンポーネントを取得します。
	/// </summary>
	/// <typeparam name="T">取得したいコンポーネントを指定します。</typeparam>
	/// <param name="e">コンポーネント取得先のエンティティ</param>
	/// <returns>取得したコンポーネントをポインタ型で返します。</returns>
	template <typename T> T* GetComponent(Entity e) { return m_cm.GetComponent<T>(e); }

	/// <summary>
	/// 特定のエンティティが指定したコンポーネントを所持しているか確認します。
	/// </summary>
	/// <typeparam name="T">確認したいコンポーネントを指定します。</typeparam>
	/// <param name="e">確認したいエンティティ</param>
	/// <returns>所持していたら true 所持していなければ false を返します。</returns>
	template <typename T> bool HasComponent(Entity e) { return m_cm.HasComponent<T>(e); }

	/// <summary>
	/// 指定したシステムを追加します。
	/// </summary>
	/// <param name="sys">make_unique<追加したいSystem>で指定します。</param>
	void AddSystem(std::unique_ptr<System> sys);

	/// <summary>
	/// 指定したシステムを取得します。
	/// </summary>
	/// <typeparam name="T">取得したいSystemを指定します。（Systemクラスを継承しているものだけです。）</typeparam>
	/// <returns>取得したシステムをポインタ型で返します。</returns>
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
	/// ワールドにあるすべてのUpdateを管理します。
	/// 毎秒約60回更新され、Drawよりも先に処理が行われます。
	/// </summary>
	void Update(World& world);

	/// <summary>
	/// ワールドにある全てのDrawを管理します。
	/// 毎秒約60回更新され、Updateよりも後に処理が行われます。
	/// </summary>
	/// <param name="world"></param>
	void Draw(World& world);
};