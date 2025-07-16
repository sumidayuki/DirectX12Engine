#pragma once

#include "Mesh.h"
#include "Material.h"
#include <string>
#include <vector>

// Forward declarations for Assimp types
// Assimpの型を前方宣言
struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;

/// <summary>
/// Imports Mesh and Material assets from 3D model files.
/// 3DモデルファイルからMeshとMaterialのアセットを読み込みます。
/// </summary>
class ModelImporter
{
public:
    std::vector<ComPtr<Mesh>> meshes;
    std::vector<ComPtr<Material>> materials;

public:
    ModelImporter() = default;

    /// <summary>
    /// Loads a model file from the specified path.
    /// On success, the loaded meshes and materials are stored in the member variables.
    /// 指定されたパスのモデルファイルを読み込みます。
    /// 成功すると、読み込んだメッシュとマテリアルがメンバ変数に格納されます。
    /// </summary>
    /// <param name="path">Path to the model file</param>
    /// <returns>True on success, false otherwise</returns>
    bool Import(const std::string& path);

private:
    std::string m_directory;

    void ProcessNode(aiNode* node, const aiScene* scene);
    ComPtr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene);

    // 新しい実装に合わせて更新
    ComPtr<Material> ProcessMaterial(aiMaterial* mat, const aiScene* scene);
};