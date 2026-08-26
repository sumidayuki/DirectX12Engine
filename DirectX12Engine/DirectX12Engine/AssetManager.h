#pragma once

enum class AssetType
{
	Texture,
	Model,
	Audio,
	Effect
};

// aiNodeの代替。エンティティ階層を表します。
struct PrefabNode
{
	std::string name;
	Matrix4x4 transform;
	std::vector<int> meshIndices;
	std::vector<std::unique_ptr<PrefabNode>> children;
};

// ロードされたモデルのリソースと階層構造を保持します。
struct ModelData
{
	std::wstring path;
	std::vector<Mesh*> meshes;
	std::vector<Material*> materials;
	Skeleton* skeleton = nullptr;
	std::unordered_map<std::string, Animation*> animations;
	std::unique_ptr<PrefabNode> rootNode;
};

class AssetManager : public Singleton<AssetManager>
{
	friend class Singleton<AssetManager>;

private:
	TextureImporter m_texImp;
	ModelImporter m_modImp;
	AudioImporter m_audImp;
	EffectImporter m_effImp;

	std::unordered_map<std::wstring, ComPtr<Texture2D>> m_textureCache;
	std::unordered_map<std::wstring, ModelData*> m_modelCache;
	std::unordered_map<std::wstring, AudioClip*> m_audioCache;
	std::unordered_map<std::wstring, Effect*> m_effectCache;

private:
	AssetManager();
	~AssetManager();

public:
	/// <summary>
	/// 指定したタイプのアセットを対応するキャッシュにロードします。
	/// </summary>
	/// <param name="type"></param>
	/// <param name="path"></param>
	void LoadAsset(AssetType type, const std::wstring& path);

	/// <summary>
	/// 指定したディレクトリ内のすべてのアセットを指定したタイプでロードします。
	/// </summary>
	/// <param name="type"></param>
	/// <param name="directoryPath"></param>
	void LoadAssetsFromDirectory(AssetType type, const std::wstring& directoryPath);

	/// <summary>
	/// 指定したタイプのアセットをキャッシュから取得します。
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="type"></param>
	/// <param name="path"></param>
	/// <returns></returns>
	template <typename T>
	T* GetAsset(AssetType type, const std::wstring& path);

	/// <summary>
	/// キャッシュされたモデルからエンティティを生成します。
	/// </summary>
	/// <param name="path"></param>
	/// <param name="parent"></param>
	/// <param name="localPosition"></param>
	/// <param name="localRotation"></param>
	/// <returns></returns>
	Entity Instantiate(
		const std::wstring& path,
		Transform* parent = nullptr,
		const Vector3& localPosition = Vector3::zero,
		const Quaternion& localRotation = Quaternion::identity,
		LayerMask layer = Layers::Default
	);

	/// <summary>
	/// キャッシュをクリアします。
	/// </summary>
	void Clear();

private:
	// ModelDataからエンティティ階層を再帰的に生成するヘルパー
	Entity CreateEntityFromPrefab(
		const PrefabNode& node,
		const ModelData* modelData,
		Entity parentEntity,
		LayerMask layer
	);

	void BindBoneEntitiesRecursive(const Entity& entity, const Skeleton* skeleton, std::vector<Entity>& boneEntities, World& world);

	void PrintHierarchy(Transform* transform, int level);
};

template<typename T>
inline T* AssetManager::GetAsset(AssetType type, const std::wstring& path)
{
	switch (type)
	{
	case AssetType::Texture:
		if (m_textureCache.count(path))
		{
			return (T*)m_textureCache[path].Get();
		}
		break;

	case AssetType::Model:
		if (m_modelCache.count(path))
		{
			return (T*)m_modelCache[path];
		}
		break;

	case AssetType::Audio:
		if (m_audioCache.count(path))
		{
			return (T*)m_audioCache[path];
		}
		break;

	case AssetType::Effect:
		if (m_effectCache.count(path))
		{
			return (T*)m_effectCache[path];
		}
		break;
	}

	return nullptr;
}
