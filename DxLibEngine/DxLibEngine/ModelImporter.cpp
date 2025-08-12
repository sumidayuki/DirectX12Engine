// ModelImporter.cpp
#include "Precompiled.h"
#include "ModelImporter.h"
#include "TextureImporter.h" // 外部テクスチャの読み込みに利用
#include "World.h"           // Worldクラスの定義をインクルード

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <filesystem> // C++17 filesystem: パス操作に便利

// string (UTF-8) と wstring を相互変換するユーティリティ
static std::string WStringToString(const std::wstring& wstr)
{
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

// デフォルトコンストラクタ
ModelImporter::ModelImporter()
    : m_globalScale(1.0f)
    , m_calculateTangents(true)
    , m_generateNormals(true)
    , m_flipUVs(true)
    , m_joinIdenticalVertices(true)
    , m_importMaterials(true)
{
}

// パスを指定してインポートするユーティリティ関数
ComPtr<Model> ModelImporter::Import(const std::wstring& path, World& world)
{
    SetAssetPath(path.c_str());
    return Import(world);
}

// メインのインポート関数
ComPtr<Model> ModelImporter::Import(World& world)
{
    Assimp::Importer importer;

    // 設定に基づいて動的に前処理フラグを構築
    unsigned int flags = aiProcess_Triangulate;
    if (m_flipUVs)                 flags |= aiProcess_FlipUVs;
    if (m_generateNormals)         flags |= aiProcess_GenSmoothNormals;
    if (m_calculateTangents)       flags |= aiProcess_CalcTangentSpace;
    if (m_joinIdenticalVertices)   flags |= aiProcess_JoinIdenticalVertices;
    // 必要に応じて他のフラグも追加できます (例: aiProcess_OptimizeMeshes)

    const std::string path_s = WStringToString(GetAssetPath());
    if (path_s.empty())
    {
        OutputDebugStringA("ModelImporter Error: Asset path is not set or empty.\n");
        return nullptr;
    }

    const aiScene* scene = importer.ReadFile(path_s, flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        OutputDebugStringA(("ASSIMP_ERROR: " + std::string(importer.GetErrorString()) + "\n").c_str());
        return nullptr;
    }

    ComPtr<Model> modelData;
    modelData.Attach(new Model());

    // マテリアルの読み込み
    if (m_importMaterials && scene->mNumMaterials > 0)
    {
        modelData->m_materials.resize(scene->mNumMaterials);
        for (unsigned int i = 0; i < scene->mNumMaterials; i++)
        {
            modelData->m_materials[i] = ProcessMaterial(scene->mMaterials[i], scene, world);
        }
    }

    // ノードとメッシュの処理
    ProcessNode(scene->mRootNode, scene, modelData.Get());

    return modelData;
}

// ノードを再帰的に処理
void ModelImporter::ProcessNode(aiNode* node, const aiScene* scene, Model* modelData)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        modelData->m_meshes.push_back(ProcessMesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene, modelData);
    }
}

// メッシュデータを抽出
ComPtr<Mesh> ModelImporter::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Mesh::Vertex> vertices(mesh->mNumVertices);
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        // 位置 (グローバルスケールを適用)
        vertices[i].position = {
            mesh->mVertices[i].x * m_globalScale,
            mesh->mVertices[i].y * m_globalScale,
            mesh->mVertices[i].z * m_globalScale
        };

        // 法線
        if (mesh->HasNormals()) {
            vertices[i].normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
        }

        // UV座標
        if (mesh->HasTextureCoords(0)) {
            vertices[i].uv = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
        }
        else {
            vertices[i].uv = { 0.0f, 0.0f };
        }

        // 接線
        if (mesh->HasTangentsAndBitangents()) {
            vertices[i].tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
        }
    }

    // インデックス
    std::vector<uint32_t> indices;
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    ComPtr<Mesh> newMesh;
    newMesh.Attach(new Mesh());
    newMesh->SetupMesh(vertices, indices);
    newMesh->SetMaterialIndex(mesh->mMaterialIndex);

    return newMesh;
}

// マテリアルデータを抽出
ComPtr<Material> ModelImporter::ProcessMaterial(aiMaterial* mat, const aiScene* scene, World& world)
{
    ComPtr<Material> newMaterial;
    newMaterial.Attach(new Material());
    DescriptorAllocator* srvAllocator = world.GetSrvAllocator();

    // 色のプロパティ
    aiColor4D color;
    if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &color) == AI_SUCCESS) {
        newMaterial->SetDiffuseColor({ color.r, color.g, color.b, color.a });
    }
    // (他の色: Specular, Ambientなども同様に)

    // Diffuseテクスチャの処理
    if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0)
    {
        aiString path;
        mat->GetTexture(aiTextureType_DIFFUSE, 0, &path);
        ComPtr<Texture2D> diffuseTexture;
        TextureImporter textureImporter;

        const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(path.C_Str());
        if (embeddedTexture)
        {
            // 埋め込みテクスチャをメモリからロード
            if (embeddedTexture->mHeight == 0) { // 圧縮形式
                diffuseTexture.Attach(textureImporter.Import(embeddedTexture->pcData, embeddedTexture->mWidth));
            }
            else { // 非圧縮形式
                diffuseTexture.Attach(textureImporter.Import(
                    embeddedTexture->mWidth, embeddedTexture->mHeight, DXGI_FORMAT_R8G8B8A8_UNORM,
                    embeddedTexture->pcData, embeddedTexture->mWidth * sizeof(aiTexel)));
            }
        }
        else
        {
            // 外部テクスチャをファイルパスから解決してロード
            std::filesystem::path modelPath = GetAssetPath();
            std::filesystem::path texturePath = modelPath.parent_path() / path.C_Str();

            if (std::filesystem::exists(texturePath)) {
                diffuseTexture.Attach(textureImporter.Import(texturePath.c_str()));
            }
            else {
                OutputDebugStringW((L"ModelImporter Warning: External texture not found at " + texturePath.wstring() + L"\n").c_str());
            }
        }

        if (diffuseTexture) {
            newMaterial->SetTexture(Material::TextureSlot::Diffuse, diffuseTexture.Get(), srvAllocator);
        }
    }

    // 他のテクスチャタイプ (NORMAL, SPECULARなど) も同様に処理を追加できます。

    // デフォルトテクスチャの設定
    if (!newMaterial->GetTexture(Material::TextureSlot::Diffuse)) {
        newMaterial->SetTexture(Material::TextureSlot::Diffuse, MeshRendererSystem::GetDefaultWhiteTexture(), srvAllocator);
    }

    return newMaterial;
}