#pragma once

#include "Mesh.h"
#include "Material.h"
#include <string>
#include <vector>

// Forward declarations for Assimp types
// Assimp�̌^��O���錾
struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;

/// <summary>
/// Imports Mesh and Material assets from 3D model files.
/// 3D���f���t�@�C������Mesh��Material�̃A�Z�b�g��ǂݍ��݂܂��B
/// </summary>
class ModelImporter
{
public:
    std::vector<ComPtr<Mesh>> meshes;
    std::vector<ComPtr<Material>> materials;

public:
    ModelImporter() = default;

    /// <summary>
    /// Loads a model file from the specified path.
    /// On success, the loaded meshes and materials are stored in the member variables.
    /// �w�肳�ꂽ�p�X�̃��f���t�@�C����ǂݍ��݂܂��B
    /// ��������ƁA�ǂݍ��񂾃��b�V���ƃ}�e���A���������o�ϐ��Ɋi�[����܂��B
    /// </summary>
    /// <param name="path">Path to the model file</param>
    /// <returns>True on success, false otherwise</returns>
    bool Import(const std::string& path);

private:
    std::string m_directory;

    void ProcessNode(aiNode* node, const aiScene* scene);
    ComPtr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene);

    // �V���������ɍ��킹�čX�V
    ComPtr<Material> ProcessMaterial(aiMaterial* mat, const aiScene* scene);
};