#pragma once

// Assimpの型を前方宣言
struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;

/// <summary>
/// 3DモデルファイルからMeshとMaterialのアセットを読み込みます。
/// 柔軟な設定が可能で、再利用性の高い設計になっています。
/// </summary>
class ModelImporter : public AssetImporter
{
private:
    float m_globalScale;
    bool m_calculateTangents;
    bool m_generateNormals;
    bool m_flipUVs;
    bool m_joinIdenticalVertices;
    bool m_importMaterials;

    static std::unordered_map<std::wstring, ComPtr<Model>> s_modelCache;

public:
    ModelImporter();
    virtual ~ModelImporter() override = default;

    /// <summary>
    /// 設定されたアセットパスからモデルファイルを読み込みます。
    /// </summary>
    /// <param name="world">マテリアルのSRVアロケータ等を取得するために使用します。</param>
    /// <returns>成功した場合はModelへのComPtr、失敗した場合はnullptrを返します。</returns>
    Entity* Import(World& world);

    /// <summary>
    /// パスを直接指定してモデルファイルを読み込むためのユーティリティ関数です。
    /// </summary>
    Entity* Import(const std::wstring& path, World& world);

    /// <summary> モデル全体のスケールを設定します。 </summary>
    void SetGlobalScale(float scale) { m_globalScale = scale; }
    float GetGlobalScale() const { return m_globalScale; }

    /// <summary> 接線と従法線を計算するか設定します。 </summary>
    void SetCalculateTangents(bool enabled) { m_calculateTangents = enabled; }
    bool GetCalculateTangents() const { return m_calculateTangents; }

    /// <summary> 法線が存在しない場合に生成するか設定します。 </summary>
    void SetGenerateNormals(bool enabled) { m_generateNormals = enabled; }
    bool GetGenerateNormals() const { return m_generateNormals; }

    /// <summary> UV座標のY軸を反転させるか設定します。 </summary>
    void SetFlipUVs(bool enabled) { m_flipUVs = enabled; }
    bool GetFlipUVs() const { return m_flipUVs; }

    /// <summary> 同一頂点を結合して最適化するか設定します。 </summary>
    void SetJoinIdenticalVertices(bool enabled) { m_joinIdenticalVertices = enabled; }
    bool GetJoinIdenticalVertices() const { return m_joinIdenticalVertices; }

    /// <summary> マテリアルをインポートするか設定します。 </summary>
    void SetImportMaterials(bool enabled) { m_importMaterials = enabled; }
    bool GetImportMaterials() const { return m_importMaterials; }

private:
    // --- ヘルパー関数 ---
    void ProcessNode(aiNode* node, const aiScene* scene, Mesh* mesh, Skeleton* skeleton);

    void ProcessAnimations(const aiScene* scene, std::vector<ComPtr<Animation>>& animations);

    void ReadSkeletonHierarchy(Bone* parentBone, const aiNode* node);

    void LoadBones(std::vector<Mesh::Vertex>& vertices, aiMesh* mesh, Skeleton* skeleton);

    ComPtr<Material> ProcessMaterial(aiMaterial* mat, const aiScene* scene, DescriptorAllocator* srvAllocator);
};