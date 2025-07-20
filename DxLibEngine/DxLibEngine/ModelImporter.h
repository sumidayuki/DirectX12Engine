#pragma once

// Assimp�̌^��O���錾
struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;

/// <summary>
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
    /// �w�肳�ꂽ�p�X�̃��f���t�@�C����ǂݍ��݂܂��B
    /// ��������ƁA�ǂݍ��񂾃��b�V���ƃ}�e���A���������o�ϐ��Ɋi�[����܂��B
    /// </summary>
    /// <param name="path"></param>
    /// <returns></returns>
    bool Import(const std::string& path);

private:
    std::string m_directory;

    void ProcessNode(aiNode* node, const aiScene* scene);
    ComPtr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene);

    ComPtr<Material> ProcessMaterial(aiMaterial* mat, const aiScene* scene);
};