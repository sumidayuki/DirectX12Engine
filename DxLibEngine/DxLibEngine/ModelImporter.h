// ModelImporter.h
#pragma once
#include "AssetImporter.h"
#include "Model.h" // Modelクラスの定義をインクルード

// Assimpの型を前方宣言
struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;

// Worldクラスを前方宣言
class World;

/// <summary>
/// 3DモデルファイルからMeshとMaterialのアセットを読み込みます。
/// 柔軟な設定が可能で、再利用性の高い設計になっています。
/// </summary>
class ModelImporter : public AssetImporter
{
public:
    ModelImporter();
    virtual ~ModelImporter() override = default;

    /// <summary>
    /// 設定されたアセットパスからモデルファイルを読み込みます。
    /// </summary>
    /// <param name="world">マテリアルのSRVアロケータ等を取得するために使用します。</param>
    /// <returns>成功した場合はModelへのComPtr、失敗した場合はnullptrを返します。</returns>
    ComPtr<Model> Import(World& world);

    /// <summary>
    /// パスを直接指定してモデルファイルを読み込むためのユーティリティ関数です。
    /// </summary>
    ComPtr<Model> Import(const std::wstring& path, World& world);

    // --- インポート設定 ---

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
    // --- メンバ変数 ---
    float m_globalScale;
    bool m_calculateTangents;
    bool m_generateNormals;
    bool m_flipUVs;
    bool m_joinIdenticalVertices;
    bool m_importMaterials;

private:
    // --- ヘルパー関数 ---
    void ProcessNode(aiNode* node, const aiScene* scene, Model* modelData);
    ComPtr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene);
    ComPtr<Material> ProcessMaterial(aiMaterial* mat, const aiScene* scene, World& world);
};