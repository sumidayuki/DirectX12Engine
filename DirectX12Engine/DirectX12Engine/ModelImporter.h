#pragma once

// Assimp�̌^��O���錾
struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;

/// <summary>
/// 3D���f���t�@�C������Mesh��Material�̃A�Z�b�g��ǂݍ��݂܂��B
/// �_��Ȑݒ肪�\�ŁA�ė��p���̍����݌v�ɂȂ��Ă��܂��B
/// </summary>
class ModelImporter : public AssetImporter
{
private:
    float m_globalScale;
    bool m_calculateTangents;
    bool m_generateNormals;
    bool m_flipUVs;
    bool m_joinIdenticalVertices;
    bool m_importMaterials;

    static std::unordered_map<std::wstring, ComPtr<Model>> s_modelCache;

public:
    ModelImporter();
    virtual ~ModelImporter() override = default;

    /// <summary>
    /// �ݒ肳�ꂽ�A�Z�b�g�p�X���烂�f���t�@�C����ǂݍ��݂܂��B
    /// </summary>
    /// <param name="world">�}�e���A����SRV�A���P�[�^�����擾���邽�߂Ɏg�p���܂��B</param>
    /// <returns>���������ꍇ��Model�ւ�ComPtr�A���s�����ꍇ��nullptr��Ԃ��܂��B</returns>
    Entity* Import(World& world);

    /// <summary>
    /// �p�X�𒼐ڎw�肵�ă��f���t�@�C����ǂݍ��ނ��߂̃��[�e�B���e�B�֐��ł��B
    /// </summary>
    Entity* Import(const std::wstring& path, World& world);

    /// <summary> ���f���S�̂̃X�P�[����ݒ肵�܂��B </summary>
    void SetGlobalScale(float scale) { m_globalScale = scale; }
    float GetGlobalScale() const { return m_globalScale; }

    /// <summary> �ڐ��Ə]�@�����v�Z���邩�ݒ肵�܂��B </summary>
    void SetCalculateTangents(bool enabled) { m_calculateTangents = enabled; }
    bool GetCalculateTangents() const { return m_calculateTangents; }

    /// <summary> �@�������݂��Ȃ��ꍇ�ɐ������邩�ݒ肵�܂��B </summary>
    void SetGenerateNormals(bool enabled) { m_generateNormals = enabled; }
    bool GetGenerateNormals() const { return m_generateNormals; }

    /// <summary> UV���W��Y���𔽓]�����邩�ݒ肵�܂��B </summary>
    void SetFlipUVs(bool enabled) { m_flipUVs = enabled; }
    bool GetFlipUVs() const { return m_flipUVs; }

    /// <summary> ���꒸�_���������čœK�����邩�ݒ肵�܂��B </summary>
    void SetJoinIdenticalVertices(bool enabled) { m_joinIdenticalVertices = enabled; }
    bool GetJoinIdenticalVertices() const { return m_joinIdenticalVertices; }

    /// <summary> �}�e���A�����C���|�[�g���邩�ݒ肵�܂��B </summary>
    void SetImportMaterials(bool enabled) { m_importMaterials = enabled; }
    bool GetImportMaterials() const { return m_importMaterials; }

private:
    // --- �w���p�[�֐� ---
    void ProcessNode(aiNode* node, const aiScene* scene, Mesh* mesh, Skeleton* skeleton);

    void ProcessAnimations(const aiScene* scene, std::vector<ComPtr<Animation>>& animations);

    void ReadSkeletonHierarchy(Bone* parentBone, const aiNode* node);

    void LoadBones(std::vector<Mesh::Vertex>& vertices, aiMesh* mesh, Skeleton* skeleton);

    ComPtr<Material> ProcessMaterial(aiMaterial* mat, const aiScene* scene, DescriptorAllocator* srvAllocator);
};