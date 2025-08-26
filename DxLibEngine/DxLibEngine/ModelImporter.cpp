#include "Precompiled.h"
#include "ModelImporter.h"
#include "TextureImporter.h" // �O���e�N�X�`���̓ǂݍ��݂ɗ��p
#include "World.h"           // World�N���X�̒�`���C���N���[�h

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <filesystem> // C++17 filesystem: �p�X����ɕ֗�

// string (UTF-8) �� wstring �𑊌ݕϊ����郆�[�e�B���e�B
static std::string WStringToString(const std::wstring& wstr)
{
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

// �f�t�H���g�R���X�g���N�^
ModelImporter::ModelImporter()
    : m_globalScale(1.0f)
    , m_calculateTangents(true)
    , m_generateNormals(true)
    , m_flipUVs(true)
    , m_joinIdenticalVertices(true)
    , m_importMaterials(true)
{
}

// �p�X���w�肵�ăC���|�[�g���郆�[�e�B���e�B�֐�
ComPtr<Model> ModelImporter::Import(const std::wstring& path, World& world)
{
    SetAssetPath(path.c_str());
    return Import(world);
}

// ���C���̃C���|�[�g�֐�
ComPtr<Model> ModelImporter::Import(World& world)
{
    Assimp::Importer importer;

    // �ݒ�Ɋ�Â��ē��I�ɑO�����t���O���\�z
    unsigned int flags = aiProcess_Triangulate;
    if (m_flipUVs)                 flags |= aiProcess_FlipUVs;
    if (m_generateNormals)         flags |= aiProcess_GenSmoothNormals;
    if (m_calculateTangents)       flags |= aiProcess_CalcTangentSpace;
    if (m_joinIdenticalVertices)   flags |= aiProcess_JoinIdenticalVertices;
    // �K�v�ɉ����đ��̃t���O���ǉ��ł��܂� (��: aiProcess_OptimizeMeshes)

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

    // �}�e���A���̓ǂݍ���
    if (m_importMaterials && scene->mNumMaterials > 0)
    {
        modelData->m_materials.resize(scene->mNumMaterials);
        for (unsigned int i = 0; i < scene->mNumMaterials; i++)
        {
            modelData->m_materials[i] = ProcessMaterial(scene->mMaterials[i], scene, world.GetSrvAllocator());
        }
    }

    modelData->m_mesh.Attach(new Mesh());

    // �m�[�h���ċA�I�ɏ������A���ׂẴ��b�V���f�[�^��P��̃x�N�g���ɏW��
    ProcessNode(scene->mRootNode, scene, modelData.Get());

    modelData->m_mesh->SetupMesh();

    return modelData;
}

// �m�[�h���ċA�I�ɏ���
void ModelImporter::ProcessNode(aiNode* node, const aiScene* scene, Model* modelData)
{
    // ���̃m�[�h�ɑ�����S�Ẵ��b�V��������
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

        const UINT startIndex = modelData->m_mesh->GetTotalIndexCount();

        // ���_�ƃC���f�b�N�X�̃f�[�^���ꎞ�I�Ɋi�[����x�N�g��
        std::vector<Mesh::Vertex> vertices;
        std::vector<uint32_t> indices;

        // ���_�𒊏o
        vertices.resize(mesh->mNumVertices);
        for (unsigned int v = 0; v < mesh->mNumVertices; v++)
        {
            vertices[v].position = {
                mesh->mVertices[v].x * m_globalScale,
                mesh->mVertices[v].y * m_globalScale,
                mesh->mVertices[v].z * m_globalScale
            };
            if (mesh->HasNormals()) {
                vertices[v].normal = { mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z };
            }
            if (mesh->HasTextureCoords(0)) {
                vertices[v].uv = { mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y };
            }
            if (mesh->HasTangentsAndBitangents()) {
                vertices[v].tangent = { mesh->mTangents[v].x, mesh->mTangents[v].y, mesh->mTangents[v].z };
            }
        }

        // �C���f�b�N�X�𒊏o
        for (unsigned int f = 0; f < mesh->mNumFaces; f++)
        {
            aiFace face = mesh->mFaces[f];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        modelData->m_mesh->AddVertices(vertices);
        modelData->m_mesh->AddIndices(indices);

        // �T�u���b�V������V����Mesh�ɒǉ�
        modelData->m_mesh->AddSubMesh(startIndex, (UINT)indices.size(), mesh->mMaterialIndex);
    }

    // �q�m�[�h���ċA�I�ɏ���
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene, modelData);
    }
}

// �}�e���A���f�[�^�𒊏o
ComPtr<Material> ModelImporter::ProcessMaterial(aiMaterial* mat, const aiScene* scene, DescriptorAllocator* srvAllocator)
{
    ComPtr<Material> newMaterial;
    newMaterial.Attach(new Material());

    // �F�̃v���p�e�B
    aiColor4D color;
    if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &color) == AI_SUCCESS) {
        newMaterial->SetDiffuseColor({ color.r, color.g, color.b, color.a });
    }
    // (���̐F: Specular, Ambient�Ȃǂ����l��)

    // Diffuse�e�N�X�`���̏���
    if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0)
    {
        aiString path;
        mat->GetTexture(aiTextureType_DIFFUSE, 0, &path);
        ComPtr<Texture2D> diffuseTexture;
        TextureImporter textureImporter;

        const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(path.C_Str());
        if (embeddedTexture)
        {
            // ���ߍ��݃e�N�X�`�������������烍�[�h
            if (embeddedTexture->mHeight == 0) { // ���k�`��
                diffuseTexture.Attach(textureImporter.Import(embeddedTexture->pcData, embeddedTexture->mWidth));
            }
            else { // �񈳏k�`��
                diffuseTexture.Attach(textureImporter.Import(
                    embeddedTexture->mWidth, embeddedTexture->mHeight, DXGI_FORMAT_R8G8B8A8_UNORM,
                    embeddedTexture->pcData, embeddedTexture->mWidth * sizeof(aiTexel)));
            }
        }
        else
        {
            // �O���e�N�X�`�����t�@�C���p�X����������ă��[�h
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

    return newMaterial;
}