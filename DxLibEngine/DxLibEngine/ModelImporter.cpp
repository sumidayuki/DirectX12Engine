#include "Precompiled.h"
#include "ModelImporter.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// For robust path handling (C++17)
// C++17のfilesystemでパスを堅牢に扱う
#include <filesystem>

// ヘルパー関数の前方宣言
std::vector<ComPtr<Texture2D>> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& directory);

bool ModelImporter::Import(const std::string& path)
{
    // Clear previous data
    // メンバ変数をクリア
    meshes.clear();
    materials.clear();

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        OutputDebugStringA(("ASSIMP_ERROR: " + std::string(importer.GetErrorString())).c_str());
        return false;
    }

    // Store the directory path for loading textures
    // テクスチャを読み込むために、モデルファイルがあるディレクトリのパスを保存しておく
    m_directory = std::filesystem::path(path).parent_path().string();

    // First, process all materials
    // まず、すべてのマテリアルを読み込む
    materials.resize(scene->mNumMaterials);
    for (unsigned int i = 0; i < scene->mNumMaterials; i++)
    {
        materials[i] = ProcessMaterial(scene->mMaterials[i]);
    }

    // Then, process all nodes and their meshes recursively
    // 次に、すべてのメッシュを再帰的に読み込む
    ProcessNode(scene->mRootNode, scene);

    return true;
}

void ModelImporter::ProcessNode(aiNode* node, const aiScene* scene)
{
    // Process all the node's meshes
    // ノードに含まれるすべてのメッシュを処理
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(ProcessMesh(mesh, scene));
    }

    // Then do the same for each of its children
    // 子ノードに対しても再帰的に同じ処理を行う
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

ComPtr<Mesh> ModelImporter::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Mesh::Vertex> vertices(mesh->mNumVertices);
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        // 位置、法線、UV座標の抽出 (変更なし)
        vertices[i].position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };

        if (mesh->HasNormals())
        {
            vertices[i].normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
        }

        if (mesh->HasTextureCoords(0))
        {
            vertices[i].uv = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
        }
        else
        {
            vertices[i].uv = { 0.0f, 0.0f };
        }

        // --- tangentの抽出処理を追加 ---
        if (mesh->HasTangentsAndBitangents())
        {
            vertices[i].tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
        }
    }

    // インデックスの抽出とMeshのセットアップ (変更なし)
    std::vector<uint32_t> indices;

    ComPtr<Mesh> newMesh;
    newMesh.Attach(new Mesh());
    newMesh->SetupMesh(vertices, indices);
    newMesh->SetMaterialIndex(mesh->mMaterialIndex);

    return newMesh;
}

ComPtr<Material> ModelImporter::ProcessMaterial(aiMaterial* mat)
{
    ComPtr<Material> newMaterial;
    newMaterial.Attach(new Material());

    aiColor4D color;

    // 色のプロパティを取得
    if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &color) == AI_SUCCESS)
    {
        newMaterial->SetDiffuseColor({ color.r, color.g, color.b, color.a });
    }
    if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &color) == AI_SUCCESS)
    {
        newMaterial->SetSpecularColor({ color.r, color.g, color.b, color.a });
    }

    // Diffuse Map (Albedo)
    auto diffuseMaps = LoadMaterialTextures(mat, aiTextureType_DIFFUSE, m_directory);
    if (!diffuseMaps.empty())
    {
        newMaterial->SetTexture(Material::TextureSlot::Diffuse, diffuseMaps[0].Get());
    }
    else
    {
        // 失敗したら白いテクスチャを設定
        newMaterial->SetTexture(Material::TextureSlot::Diffuse, MeshRendererSystem::GetDefaultWhiteTexture());
    }

    // Normal Map
    auto normalMaps = LoadMaterialTextures(mat, aiTextureType_HEIGHT, m_directory);
    if (normalMaps.empty()) { normalMaps = LoadMaterialTextures(mat, aiTextureType_NORMALS, m_directory); }
    if (!normalMaps.empty())
    {
        newMaterial->SetTexture(Material::TextureSlot::Normal, normalMaps[0].Get());
    }
    else
    {
        // NOTE: 法線マップのデフォルトは、通常(0.5, 0.5, 1.0)を表す青色ですが、
        // ここでは簡略化のため白を使います。これにより法線マッピングは無効になります。
        OutputDebugStringA("白テクスチャ");

        newMaterial->SetTexture(Material::TextureSlot::Normal, MeshRendererSystem::GetDefaultWhiteTexture());
    }

    // Specular Map
    auto specularMaps = LoadMaterialTextures(mat, aiTextureType_SPECULAR, m_directory);
    if (!specularMaps.empty())
    {
        newMaterial->SetTexture(Material::TextureSlot::Specular, specularMaps[0].Get());
    }
    else
    {
        OutputDebugStringA("白テクスチャ");

        // スペキュラマップがない場合は、黒または白のテクスチャを設定
        newMaterial->SetTexture(Material::TextureSlot::Specular, MeshRendererSystem::GetDefaultWhiteTexture());
    }

    return newMaterial;
}

// 特定の種類のテクスチャをマテリアルから読み込むヘルパー関数
std::vector<ComPtr<Texture2D>> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& directory)
{
    std::vector<ComPtr<Texture2D>> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        std::filesystem::path filePath = std::filesystem::path(str.C_Str());

        // 構築したパスをTextureImporterに渡します。
        //    ファイルが存在しない等の問題は、TextureImporterがnullptrを返して対処します。
        TextureImporter texImporter;
        ComPtr<Texture2D> texture;
        texture.Attach(texImporter.Import(filePath.c_str()));

        // TextureImporterが成功した場合（textureがnullptrでない場合）のみ、リストに追加します。
        if (texture)
        {
            textures.push_back(texture);
        }
    }
    return textures;
}