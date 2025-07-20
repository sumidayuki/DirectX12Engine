#pragma once

// Assimpの型を前方宣言
struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;

/// <summary>
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
    /// 指定されたパスのモデルファイルを読み込みます。
    /// 成功すると、読み込んだメッシュとマテリアルがメンバ変数に格納されます。
    /// </summary>
    /// <param name="path"></param>
    /// <returns></returns>
    bool Import(const std::string& path);

private:
    std::string m_directory;

    void ProcessNode(aiNode* node, const aiScene* scene);
    ComPtr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene);

    ComPtr<Material> ProcessMaterial(aiMaterial* mat, const aiScene* scene);
};