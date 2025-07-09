#include "ModelImporter.h"

// Assimp�̃w�b�_���C���N���[�h
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

std::vector<ComPtr<Mesh>> ModelImporter::Import(const std::string& path)
{
    std::vector<ComPtr<Mesh>> meshes;

    Assimp::Importer importer;
    // postprocess�t���O�ŁA�ǂݍ��ݎ��ɍs���ǉ��������w��ł��܂��B
    // aiProcess_Triangulate: �S�Ă̖ʂ��O�p�`�ɂ���
    // aiProcess_FlipUVs: �e�N�X�`����UV���W���㉺���]������i�����̃��f���ŕK�v�j
    // aiProcess_CalcTangentSpace: �@���}�b�v�̂��߂̐ڋ�Ԃ��v�Z����
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        // �G���[���O
        std::string errorMsg = "Assimp Error: ";
        errorMsg += importer.GetErrorString();
        OutputDebugStringA(errorMsg.c_str());
        return meshes; // ��̃��X�g��Ԃ�
    }

    // ���[�g�m�[�h����ċA�I�ɏ������J�n
    ProcessNode(scene->mRootNode, scene, meshes);

    return meshes;
}

void ModelImporter::ProcessNode(aiNode* node, const aiScene* scene, std::vector<ComPtr<Mesh>>& meshes)
{
    // ���̃m�[�h�������ׂẴ��b�V��������
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(ProcessMesh(mesh, scene));
    }

    // ���̃m�[�h�̎q�m�[�h���ċA�I�ɏ���
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene, meshes);
    }
}

ComPtr<Mesh> ModelImporter::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Mesh::Vertex> vertices;
    std::vector<uint32_t> indices;

    // ���_�f�[�^�𒊏o
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Mesh::Vertex vertex;

        // ���W
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;

        // �@��
        if (mesh->HasNormals())
        {
            vertex.normal.x = mesh->mNormals[i].x;
            vertex.normal.y = mesh->mNormals[i].y;
            vertex.normal.z = mesh->mNormals[i].z;
        }

        // UV���W (�e�N�X�`�����W)
        // �ŏ��̃Z�b�g(0��)�̂ݎg�p�B���f���ɂ���Ă͕�����UV�Z�b�g�������Ƃ�����B
        if (mesh->HasTextureCoords(0))
        {
            vertex.uv.x = mesh->mTextureCoords[0][i].x;
            vertex.uv.y = mesh->mTextureCoords[0][i].y;
        }
        else
        {
            vertex.uv = Vector2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    // �C���f�b�N�X�f�[�^�𒊏o
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    // �G���W����Mesh�I�u�W�F�N�g���쐬���ăZ�b�g�A�b�v
    ComPtr<Mesh> newMesh;
    newMesh.Attach(new Mesh());
    newMesh->SetupMesh(vertices, indices);

    return newMesh;
}