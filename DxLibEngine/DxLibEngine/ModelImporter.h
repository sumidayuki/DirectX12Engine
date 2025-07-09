#pragma once

#include "AssetImporter.h"
#include "Mesh.h"

// Assimp�̃w�b�_���C���N���[�h
struct aiNode;
struct aiScene;
struct aiMesh;

/// <summary>
/// 3D���f���t�@�C����ǂݍ��݁AMesh�I�u�W�F�N�g�𐶐�����N���X�ł��B
/// </summary>
class ModelImporter : public AssetImporter
{
public:
    ModelImporter() = default;
    ~ModelImporter() = default;

    /// <summary>
    /// �w�肳�ꂽ�p�X�̃��f���t�@�C����ǂݍ��݂܂��B
    /// </summary>
    /// <param name="path">���f���t�@�C���ւ̃p�X</param>
    /// <returns>�ǂݍ��񂾃��b�V���̃��X�g�B���s�����ꍇ�͋�̃��X�g��Ԃ��܂��B</returns>
    std::vector<ComPtr<Mesh>> Import(const std::string& path);

private:
    /// <summary>
    /// Assimp�̃V�[���m�[�h���ċA�I�ɏ������܂��B
    /// </summary>
    void ProcessNode(aiNode* node, const aiScene* scene, std::vector<ComPtr<Mesh>>& meshes);

    /// <summary>
    /// Assimp�̃��b�V���f�[�^���G���W����Mesh�R���|�[�l���g�ɕϊ����܂��B
    /// </summary>
    ComPtr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene);
};