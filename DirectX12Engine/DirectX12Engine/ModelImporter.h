#pragma once

struct PrefabNode;
struct ModelData;
class AssetManager;

struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;

struct Transform;

/// <summary>
/// 3DモデルファイルからMeshとMaterial、アニメーション、スケルトンのアセットを読み込むクラス。
/// Assimpライブラリを使用し、柔軟な設定が可能で再利用性の高い設計になっています。
/// </summary>
class ModelImporter : public AssetImporter
{
private:
    float m_globalScale;            // モデル全体のスケールを調整
    bool m_calculateTangents;       // 法線マップ等に用いる接線と従法線を計算するか
    bool m_generateNormals;         // 法線が存在しない場合に生成するか
    bool m_flipUVs;                 // UV座標のY軸を反転させるか（DirectXとOpenGLの座標系違いを吸収）
    bool m_joinIdenticalVertices;   // 同じ頂点を結合して最適化するか
    bool m_importMaterials;         // マテリアルとテクスチャをインポートするか

public:
    // コンストラクタ: 各設定のデフォルト値を初期化
    ModelImporter();
    // デストラクタ: 継承クラスのため仮想関数として定義
    virtual ~ModelImporter() override = default;

    ModelData* Import(const std::wstring& path);

    /// <summary>
    /// 指定されたパスのモデルファイルを読み込み、リソースをModelDataとして構築します。
    /// </summary>
    /// <returns></returns>
    ModelData* Import();

    // モデル全体のスケールを設定します。
    void SetGlobalScale(float scale) { m_globalScale = scale; }
    float GetGlobalScale() const { return m_globalScale; }

    // 接線と従法線を計算するか設定します。
    void SetCalculateTangents(bool enabled) { m_calculateTangents = enabled; }
    bool GetCalculateTangents() const { return m_calculateTangents; }

    // 法線が存在しない場合に生成するか設定します。
    void SetGenerateNormals(bool enabled) { m_generateNormals = enabled; }
    bool GetGenerateNormals() const { return m_generateNormals; }

    // UV座標のY軸を反転させるか設定します。
    void SetFlipUVs(bool enabled) { m_flipUVs = enabled; }
    bool GetFlipUVs() const { return m_flipUVs; }

    // 同一頂点を結合して最適化するか設定します。
    void SetJoinIdenticalVertices(bool enabled) { m_joinIdenticalVertices = enabled; }
    bool GetJoinIdenticalVertices() const { return m_joinIdenticalVertices; }

    // マテリアルをインポートするか設定します。
    void SetImportMaterials(bool enabled) { m_importMaterials = enabled; }
    bool GetImportMaterials() const { return m_importMaterials; }

private:
    // AssimpのシーンからModelDataの各部分を構築するヘルパー関数群
    void ProcessMaterials(const aiScene* scene, ModelData* modelData);
    void ProcessMeshes(const aiScene* scene, ModelData* modelData);
    void ProcessSkeleton(const aiScene* scene, ModelData* modelData);
    void ProcessAnimations(const aiScene* scene, ModelData* modelData);
    void ProcessAnimationEvent(const std::string& name, Animation& anim);
    void ProcessHierarchy(const aiScene* scene, ModelData* modelData);
    void ReadSkeletonHierarchy(Bone* parentBone, const aiNode* node);
    void LoadBones(std::vector<Mesh::Vertex>& vertices, aiMesh* mesh, Skeleton* skeleton);
    Material* ProcessSingleMaterial(aiMaterial* mat, const aiScene* scene);
    void BuildPrefabNode(std::unique_ptr<PrefabNode>& outNode, const aiNode* inNode, const ModelData* modelData);
};