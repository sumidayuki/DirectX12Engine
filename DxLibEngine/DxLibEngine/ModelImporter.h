#pragma once

#include "AssetImporter.h"
#include "Mesh.h"

// Assimpのヘッダをインクルード
struct aiNode;
struct aiScene;
struct aiMesh;

/// <summary>
/// 3Dモデルファイルを読み込み、Meshオブジェクトを生成するクラスです。
/// </summary>
class ModelImporter : public AssetImporter
{
public:
    ModelImporter() = default;
    ~ModelImporter() = default;

    /// <summary>
    /// 指定されたパスのモデルファイルを読み込みます。
    /// </summary>
    /// <param name="path">モデルファイルへのパス</param>
    /// <returns>読み込んだメッシュのリスト。失敗した場合は空のリストを返します。</returns>
    std::vector<ComPtr<Mesh>> Import(const std::string& path);

private:
    /// <summary>
    /// Assimpのシーンノードを再帰的に処理します。
    /// </summary>
    void ProcessNode(aiNode* node, const aiScene* scene, std::vector<ComPtr<Mesh>>& meshes);

    /// <summary>
    /// AssimpのメッシュデータをエンジンのMeshコンポーネントに変換します。
    /// </summary>
    ComPtr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene);
};