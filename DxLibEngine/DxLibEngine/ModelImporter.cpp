#include "Precompiled.h"
#include "ModelImporter.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// #include <filesystem> // �s�v�Ȃ��ߍ폜

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

    // m_directory �̍s���폜�B�t�@�C���V�X�e���Ɉˑ����Ȃ����ߕs�v�ł��B
    // m_directory = std::filesystem::path(path).parent_path().string();

    // �}�e���A���̓ǂݍ���
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

    // Diffuse�e�N�X�`���̏���
    if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0)
    {
        aiString path;
        mat->GetTexture(aiTextureType_DIFFUSE, 0, &path);

        ComPtr<Texture2D> diffuseTexture;
        TextureImporter importer;

        // �p�X������Ɉ�v����e�N�X�`�����V�[���ɖ��ߍ��܂�Ă��邩�`�F�b�N
        const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(path.C_Str());

        if (embeddedTexture != nullptr)
        {
            // ���ߍ��݃e�N�X�`�������������ꍇ
            if (embeddedTexture->mHeight == 0) // ���k�`�� (PNG, JPG�Ȃ�)
            {
                diffuseTexture.Attach(importer.Import(
                    embeddedTexture->pcData, // �f�[�^�{�̂ւ̃|�C���^
                    embeddedTexture->mWidth  // �f�[�^�̃T�C�Y (�o�C�g�P��)
                ));
            }
            else // �񈳏k�`�� (���̃s�N�Z���f�[�^)
            {
                diffuseTexture.Attach(importer.Import(
                    embeddedTexture->mWidth,
                    embeddedTexture->mHeight,
                    DXGI_FORMAT_R8G8B8A8_UNORM, // Assimp��aiTexel�͒ʏ킱�̃t�H�[�}�b�g
                    embeddedTexture->pcData,
                    embeddedTexture->mWidth * sizeof(aiTexel) // 1�s������̃o�C�g��
                ));
            }
        }
        else
        {
            // ���ߍ��݃e�N�X�`����������Ȃ������ꍇ(�O���t�@�C��)�́A
            // �G���[�̌����ɂȂ邽�߃t�@�C����T���ɍs���Ȃ��B
            // �K�v�ł���Όx�����o�͂���B
            OutputDebugStringA(("Warning: Texture is not embedded, skipping: " + std::string(path.C_Str()) + "\n").c_str());
        }

        if (diffuseTexture) {
            newMaterial->SetTexture(Material::TextureSlot::Diffuse, diffuseTexture.Get());
        }
    }

    // �t�H�[���o�b�N: �e�N�X�`�����ݒ肳��Ȃ������ꍇ�Ƀf�t�H���g�e�N�X�`�������蓖��
    if (!newMaterial->GetTexture(Material::TextureSlot::Diffuse)) {
        newMaterial->SetTexture(Material::TextureSlot::Diffuse, MeshRendererSystem::GetDefaultWhiteTexture());
    }

    return newMaterial;
}