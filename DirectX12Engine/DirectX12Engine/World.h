#pragma once
#include "SystemList.h"

/// <summary>
/// クラス名：World
/// 概要：ECSの全体を管理するクラスです。
/// 用途：エンティティの生成・破壊、コンポーネントの操作、システムの管理を行います。
/// </summary>
class World
{
	friend class Scene;

private:
	EntityManager							m_em;
	ArchetypeManager						m_am;
	std::vector<std::unique_ptr<System>>	m_systems;
	std::unique_ptr<DescriptorAllocator>	m_srvAllocator;
	CameraSystem*							m_cameraSystem;
	std::list<Entity>						m_allEntities;
	std::list<Entity>						m_allRootEntities;
	std::list<Entity>						m_allCameraEntities;
	std::list<Camera*>						m_allCameras;
	std::unordered_set<std::string>			m_entityNames;
	TransformSystem*						m_transformSystem;

private:
	void CollectDescendantsRecursive(Transform* parent, std::vector<Entity>& descendants);

public:
	World() { m_srvAllocator = std::make_unique<DescriptorAllocator>(50000, Graphics::BackBafferCount, DescriptorHeapType::CBV_SRV_UAV); }

	DescriptorAllocator* GetSrvAllocator() { return m_srvAllocator.get(); }

	TransformSystem* GetTransformSystem() { return m_transformSystem; }

	/// <summary>
	/// エンティティを作成します。
	/// </summary>
	/// <returns>Entity型で値を返します。</returns>
	Entity CreateEntity(const std::string& name = "空のエンティティ", LayerMask layer = Layers::Default);

	/// <summary>
	/// ArchetypeManagerへの参照を取得します。
	/// </summary>
	/// <returns></returns>
	ArchetypeManager& GetArchetypeManager() { return m_am; }

	Entity CreateWithLight
	(
		const LightType& type, 
		const Color& color = Color::white, 
		const float range = 1000.0f, 
		Transform* parent = nullptr, 
		const Vector3& pos = Vector3::zero, 
		const Quaternion& rot = Quaternion::identity
	);

	/// <summary>
	/// 頂点配列とインデックス配列からメッシュを生成し、エンティティを作成します。
	/// </summary>
	/// <param name="vertices"></param>
	/// <param name="indices"></param>
	/// <returns></returns>
	Entity CreateWithMesh(std::vector<Vector3>&& vertices, std::vector<uint32_t>&& indices, std::vector<Vector2>&& uvs, LayerMask layer = Layers::Default, Color color = Color::white, bool isWireframe = false, Texture2D* tex = nullptr);

	/// <summary>
	/// 一辺の長さを指定して立方体を生成します。
	/// </summary>
	/// <param name="xLength"></param>
	/// <param name="yLength"></param>
	/// <param name="zLegnth"></param>
	/// <param name="color"></param>
	/// <param name="isWireframe"></param>
	/// <returns></returns>
	Entity CreateCube(float xLength, float yLength, float zLegnth, LayerMask layer = Layers::Default, Color color = Color::white, bool isWireframe = false, Texture2D* tex = nullptr);

	Entity CreateSphere(float radius, uint16_t slices, uint16_t stacks, LayerMask layer = Layers::Default, Color color = Color::white, bool isWireframe = false, Texture2D* tex = nullptr);

	Entity CreateWithSprite

	(
		const wchar_t* path,
		const Rect& rect,
		const Vector2 pivot = Vector2::zero,
		float pixelsPerUnit = 100.0f,
		Transform* parent = nullptr,
		const Vector3& localPosition = Vector3::zero,
		const Quaternion& localRotation = Quaternion::identity
	);

	Entity CreateWithSprite
	(
		Texture2D* texture,
		const Rect& rect,
		const Vector2 pivot = Vector2::zero,
		float pixelsPerUnit = 100.0f,
		Transform* parent = nullptr,
		const Vector3& localPosition = Vector3::zero,
		const Quaternion& localRotation = Quaternion::identity
	);

	Entity CreateWithSprite
	(
		Sprite* sprite,
		Transform* parent = nullptr,
		const Vector3& localPosition = Vector3::zero,
		const Quaternion& localRotation = Quaternion::identity
	);

	Entity CreateCamera2D(float viewWidth, float viewHeight, const Vector3& localPosition = Vector3::zero, const Quaternion& localRotation = Quaternion::identity);

	/// <summary>
	/// ファイルパスから3Dモデルを読み込み、エンティティを生成します。
	/// </summary>
	/// <param name="path">モデルファイルへのパス (例: "Assets/character.fbx")</param>
	/// <param name="parent">親となるTransform</param>
	/// <param name="localPosition">ローカル座標</param>
	/// <param name="localRotation">ローカル回転</param>
	/// <returns>生成されたエンティティ。読み込みに失敗した場合はnullptr</returns>
	Entity CreateWithModel
	(
		const std::wstring& path,
		Transform* parent,
		const Vector3& localPosition,
		const Quaternion& localRotation,
		LayerMask layer = Layers::Default
	);

	Entity CreateCamera3D(float fieldOfView, float aspect, float nearClipPlane, float farClipPlane, const Vector3& localPosition = Vector3::zero, const Quaternion& localRotation = Quaternion::identity);


	/// <summary>
	/// エンティティを破壊します。
	/// </summary>
	/// <param name="entity">破壊したいEntity</param>
	void DestroyEntity(Entity entity);

	/// <summary>
	/// 指定したエンティティが生存しているかを確認します。
	/// </summary>
	/// <param name="entity">確認したいEntity</param>
	/// <returns>生存している場合は true、生存していない場合は false を返します。</returns>
	bool IsAlive(Entity entity)const { return m_em.IsAlive(entity); }

	/// <summary>
	/// 指定したタイプのコンポーネントを所持しているエンティティを取得します。
	/// この関数では最初に見つけたエンティティを返します。
	/// </summary>
	/// <typeparam name="T">コンポーネントタイプ</typeparam>
	/// <returns>見つけたエンティティ</returns>
	template<typename T> Entity FindEntityOfType();

	/// <summary>
	/// 特定のエンティティに指定したコンポーネントを追加します。
	/// </summary>
	/// <typeparam name="T">追加したいコンポーネントを指定します。</typeparam>
	/// <param name="e">コンポーネントを追加するエンティティ</param>
	/// <param name="c">追加するコンポーネントの初期状態を設定します。</param>
	template <ComponentType T> T* AddComponent(Entity e, const T& c) { return m_am.AddComponent(e, c); }

	/// <summary>
	/// 特定のエンティティから指定したコンポーネントを取得します。
	/// </summary>
	/// <typeparam name="T">取得したいコンポーネントを指定します。</typeparam>
	/// <param name="e">コンポーネント取得先のエンティティ</param>
	/// <returns>取得したコンポーネントをポインタ型で返します。</returns>
	template <ComponentType T> T* GetComponent(Entity e) { return m_am.GetComponent<T>(e); }

	/// <summary>
	/// 特定のエンティティが指定したコンポーネントを所持しているか確認します。
	/// </summary>
	/// <typeparam name="T">確認したいコンポーネントを指定します。</typeparam>
	/// <param name="e">確認したいエンティティ</param>
	/// <returns>所持していたら true 所持していなければ false を返します。</returns>
	template <ComponentType T> bool HasComponent(Entity e) { return m_am.GetComponent<T>(e) != nullptr; }

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
		return nullptr;
	}

private:
	bool Load(World& world);

	void Start(World& world);

	void BeginFrame(UINT frameIndex);

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

template<typename T>
inline Entity World::FindEntityOfType()
{
	// T のみを含む Archetype に対応する Chunk のリストを取得
	std::vector<Chunk*> chunks = m_am.GetChunks<T>();

	if (chunks.empty())
	{
		// 該当するChunkがない場合、無効なEntityを返す
		return Entity{ (EntitySize)-1, (EntitySize)-1 };
	}

	// 最初に見つかったChunk
	Chunk* firstChunk = chunks.front();

	if (firstChunk->GetCount() > 0)
	{
		// Chunk内に一つでもEntityがあれば、その最初のEntityを返す
		return firstChunk->GetEntity(0);
	}

	// Chunkが空の場合 (起こりにくいが念のため)
	return Entity{ (EntitySize)-1, (EntitySize)-1 };
}
