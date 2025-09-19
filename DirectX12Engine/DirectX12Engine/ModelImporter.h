#pragma once

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

    /// <summary>
    /// 事前に設定されたアセットパスからモデルファイルを読み込みます。
    /// </summary>
    /// <param name="world">エンティティやコンポーネントを管理するWorldクラスのインスタンス。</param>
    /// <returns>モデルのルートとなるEntityへのポインタ。失敗した場合はnullptrを返します。</returns>
    Entity* Import(World& world);

    /// <summary>
    /// ファイルパスを直接指定してモデルファイルを読み込むためのユーティリティ関数です。
    /// </summary>
    /// <param name="path">読み込むモデルファイルへのワイド文字列パス。</param>
    /// <param name="world">エンティティやコンポーネントを管理するWorldクラスのインスタンス。</param>
    /// <returns>モデルのルートとなるEntityへのポインタ。失敗した場合はnullptrを返します。</returns>
    Entity* Import(const std::wstring& path, World& world);

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
    // Assimpのノードツリーを再帰的に走査し、メッシュ、マテリアル、ボーン等を処理
    void ProcessNode(aiNode* node, const aiScene* scene, World& world, Entity* parentEntity, const std::vector<Material*>& materials, Skeleton* skeleton);

    // アニメーションデータを読み込み、独自のデータ構造に変換
    void ProcessAnimations(const aiScene* scene, std::vector<Animation*>& animations);

    // ボーン階層をAssimpのノードツリーから再帰的に読み込む
    void ReadSkeletonHierarchy(Bone* parentBone, const aiNode* node);

    // 各頂点にボーンウェイトとID情報をロード
    void LoadBones(std::vector<Mesh::Vertex>& vertices, aiMesh* mesh, Skeleton* skeleton);

    // マテリアルデータとテクスチャを読み込み、マテリアルオブジェクトを生成
    Material* ProcessMaterial(aiMaterial* mat, const aiScene* scene, DescriptorAllocator* srvAllocator);

    // インポートしたモデルの階層構造をデバッグ用にコンソール出力
    void PrintHierarchy(Transform* transform, int level);
};