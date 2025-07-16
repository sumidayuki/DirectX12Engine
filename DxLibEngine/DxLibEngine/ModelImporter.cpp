#include "Precompiled.h"
#include "ModelImporter.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// For robust path handling (C++17)
// C++17��filesystem�Ńp�X�����S�Ɉ���
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

    // �}�e���A���̓ǂݍ��� (scene��n���悤�ɕύX)
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

    // --- �C���f�b�N�X�̓ǂݍ��ݏ������C�� ---
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

    // �F�̃v���p�e�B�擾
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
        TextureImporter importer; // Importer�͈�x�����쐬

        if (path.data[0] == '*') // ���ߍ��݃e�N�X�`��
        {
            int textureIndex = std::stoi(path.C_Str() + 1);
            aiTexture* embeddedTexture = scene->mTextures[textureIndex];

            if (embeddedTexture->mHeight == 0) // ���k�`��
            {
                diffuseTexture.Attach(importer.Import(embeddedTexture->pcData, embeddedTexture->mWidth));
            }
            else // �񈳏k�`��
            {
                // ������ TextureImporter�̐V����Import�֐����Ăяo�� ������
                diffuseTexture.Attach(importer.Import(
                    embeddedTexture->mWidth,
                    embeddedTexture->mHeight,
                    DXGI_FORMAT_R8G8B8A8_UNORM,
                    embeddedTexture->pcData,
                    embeddedTexture->mWidth * sizeof(aiTexel)
                ));
            }
        }
        else // �t�@�C���p�X
        {
            std::filesystem::path finalPath = std::filesystem::path(m_directory) / std::filesystem::path(path.C_Str()).filename();
            diffuseTexture.Attach(importer.Import(finalPath.wstring().c_str()));
        }
        newMaterial->SetTexture(Material::TextureSlot::Diffuse, diffuseTexture.Get());
    }

    // �t�H�[���o�b�N
    if (!newMaterial->GetTexture(Material::TextureSlot::Diffuse)) {
        newMaterial->SetTexture(Material::TextureSlot::Diffuse, MeshRendererSystem::GetDefaultWhiteTexture());
    }

    return newMaterial;
}

// ����̎�ނ̃e�N�X�`�����}�e���A������ǂݍ��ރw���p�[�֐�
std::vector<ComPtr<Texture2D>> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& directory)
{
    std::vector<ComPtr<Texture2D>> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        std::filesystem::path filePath = std::filesystem::path(str.C_Str());

        // �\�z�����p�X��TextureImporter�ɓn���܂��B
        //    �t�@�C�������݂��Ȃ����̖��́ATextureImporter��nullptr��Ԃ��đΏ����܂��B
        TextureImporter texImporter;
        ComPtr<Texture2D> texture;
        texture.Attach(texImporter.Import(filePath.c_str()));

        // TextureImporter�����������ꍇ�itexture��nullptr�łȂ��ꍇ�j�̂݁A���X�g�ɒǉ����܂��B
        if (texture)
        {
            textures.push_back(texture);
        }
    }
    return textures;
}