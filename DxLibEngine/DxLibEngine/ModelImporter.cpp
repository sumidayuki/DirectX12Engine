#include "Precompiled.h"
#include "ModelImporter.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// For robust path handling (C++17)
// C++17��filesystem�Ńp�X�����S�Ɉ���
#include <filesystem>

// �w���p�[�֐��̑O���錾
std::vector<ComPtr<Texture2D>> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& directory);

bool ModelImporter::Import(const std::string& path)
{
    // Clear previous data
    // �����o�ϐ����N���A
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
    // �e�N�X�`����ǂݍ��ނ��߂ɁA���f���t�@�C��������f�B���N�g���̃p�X��ۑ����Ă���
    m_directory = std::filesystem::path(path).parent_path().string();

    // First, process all materials
    // �܂��A���ׂẴ}�e���A����ǂݍ���
    materials.resize(scene->mNumMaterials);
    for (unsigned int i = 0; i < scene->mNumMaterials; i++)
    {
        materials[i] = ProcessMaterial(scene->mMaterials[i]);
    }

    // Then, process all nodes and their meshes recursively
    // ���ɁA���ׂẴ��b�V�����ċA�I�ɓǂݍ���
    ProcessNode(scene->mRootNode, scene);

    return true;
}

void ModelImporter::ProcessNode(aiNode* node, const aiScene* scene)
{
    // Process all the node's meshes
    // �m�[�h�Ɋ܂܂�邷�ׂẴ��b�V��������
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(ProcessMesh(mesh, scene));
    }

    // Then do the same for each of its children
    // �q�m�[�h�ɑ΂��Ă��ċA�I�ɓ����������s��
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
        // �ʒu�A�@���AUV���W�̒��o (�ύX�Ȃ�)
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

        // --- tangent�̒��o������ǉ� ---
        if (mesh->HasTangentsAndBitangents())
        {
            vertices[i].tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
        }
    }

    // �C���f�b�N�X�̒��o��Mesh�̃Z�b�g�A�b�v (�ύX�Ȃ�)
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

    // �F�̃v���p�e�B���擾
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
        // ���s�����甒���e�N�X�`����ݒ�
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
        // NOTE: �@���}�b�v�̃f�t�H���g�́A�ʏ�(0.5, 0.5, 1.0)��\���F�ł����A
        // �����ł͊ȗ����̂��ߔ����g���܂��B����ɂ��@���}�b�s���O�͖����ɂȂ�܂��B
        OutputDebugStringA("���e�N�X�`��");

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
        OutputDebugStringA("���e�N�X�`��");

        // �X�y�L�����}�b�v���Ȃ��ꍇ�́A���܂��͔��̃e�N�X�`����ݒ�
        newMaterial->SetTexture(Material::TextureSlot::Specular, MeshRendererSystem::GetDefaultWhiteTexture());
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