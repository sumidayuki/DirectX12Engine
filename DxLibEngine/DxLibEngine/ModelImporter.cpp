#include "Precompiled.h"
#include "ModelImporter.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// For robust path handling (C++17)
// C++17のfilesystemでパスを堅牢に扱う
#include <filesystem>

bool ModelImporter::Import(const std::string& path)
{
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

    m_directory = std::filesystem::path(path).parent_path().string();

    // マテリアルの読み込み (sceneを渡すように変更)
    materials.resize(scene->mNumMaterials);
    for (unsigned int i = 0; i < scene->mNumMaterials; i++)
    {
        materials[i] = ProcessMaterial(scene->mMaterials[i], scene);
    }

    ProcessNode(scene->mRootNode, scene);

    return true;
}

void ModelImporter::ProcessNode(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(ProcessMesh(mesh, scene));
    }

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
        vertices[i].position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };

        if (mesh->HasNormals()) {
            vertices[i].normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
        }

        if (mesh->HasTextureCoords(0)) {
            vertices[i].uv = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
        }
        else {
            vertices[i].uv = { 0.0f, 0.0f };
        }

        if (mesh->HasTangentsAndBitangents()) {
            vertices[i].tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
        }
    }

    // --- インデックスの読み込み処理を修正 ---
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

ComPtr<Material> ModelImporter::ProcessMaterial(aiMaterial* mat, const aiScene* scene)
{
    ComPtr<Material> newMaterial;
    newMaterial.Attach(new Material());

    // 色のプロパティ取得
    aiColor4D color;
    if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &color) == AI_SUCCESS) {
        newMaterial->SetDiffuseColor({ color.r, color.g, color.b, color.a });
    }
    if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &color) == AI_SUCCESS) {
        newMaterial->SetSpecularColor({ color.r, color.g, color.b, color.a });
    }

    if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0)
    {
        aiString path;
        mat->GetTexture(aiTextureType_DIFFUSE, 0, &path);
        ComPtr<Texture2D> diffuseTexture;
        TextureImporter importer; // Importerは一度だけ作成

        if (path.data[0] == '*') // 埋め込みテクスチャ
        {
            int textureIndex = std::stoi(path.C_Str() + 1);
            aiTexture* embeddedTexture = scene->mTextures[textureIndex];

            if (embeddedTexture->mHeight == 0) // 圧縮形式
            {
                diffuseTexture.Attach(importer.Import(embeddedTexture->pcData, embeddedTexture->mWidth));
            }
            else // 非圧縮形式
            {
                // ★★★ TextureImporterの新しいImport関数を呼び出す ★★★
                diffuseTexture.Attach(importer.Import(
                    embeddedTexture->mWidth,
                    embeddedTexture->mHeight,
                    DXGI_FORMAT_R8G8B8A8_UNORM,
                    embeddedTexture->pcData,
                    embeddedTexture->mWidth * sizeof(aiTexel)
                ));
            }
        }
        else // ファイルパス
        {
            std::filesystem::path finalPath = std::filesystem::path(m_directory) / std::filesystem::path(path.C_Str()).filename();
            diffuseTexture.Attach(importer.Import(finalPath.wstring().c_str()));
        }
        newMaterial->SetTexture(Material::TextureSlot::Diffuse, diffuseTexture.Get());
    }

    // フォールバック
    if (!newMaterial->GetTexture(Material::TextureSlot::Diffuse)) {
        newMaterial->SetTexture(Material::TextureSlot::Diffuse, MeshRendererSystem::GetDefaultWhiteTexture());
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