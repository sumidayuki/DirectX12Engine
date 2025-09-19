#pragma once

struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;

struct Transform;

/// <summary>
/// 3D���f���t�@�C������Mesh��Material�A�A�j���[�V�����A�X�P���g���̃A�Z�b�g��ǂݍ��ރN���X�B
/// Assimp���C�u�������g�p���A�_��Ȑݒ肪�\�ōė��p���̍����݌v�ɂȂ��Ă��܂��B
/// </summary>
class ModelImporter : public AssetImporter
{
private:
    float m_globalScale;            // ���f���S�̂̃X�P�[���𒲐�
    bool m_calculateTangents;       // �@���}�b�v���ɗp����ڐ��Ə]�@�����v�Z���邩
    bool m_generateNormals;         // �@�������݂��Ȃ��ꍇ�ɐ������邩
    bool m_flipUVs;                 // UV���W��Y���𔽓]�����邩�iDirectX��OpenGL�̍��W�n�Ⴂ���z���j
    bool m_joinIdenticalVertices;   // �������_���������čœK�����邩
    bool m_importMaterials;         // �}�e���A���ƃe�N�X�`�����C���|�[�g���邩

public:
    // �R���X�g���N�^: �e�ݒ�̃f�t�H���g�l��������
    ModelImporter();
    // �f�X�g���N�^: �p���N���X�̂��߉��z�֐��Ƃ��Ē�`
    virtual ~ModelImporter() override = default;

    /// <summary>
    /// ���O�ɐݒ肳�ꂽ�A�Z�b�g�p�X���烂�f���t�@�C����ǂݍ��݂܂��B
    /// </summary>
    /// <param name="world">�G���e�B�e�B��R���|�[�l���g���Ǘ�����World�N���X�̃C���X�^���X�B</param>
    /// <returns>���f���̃��[�g�ƂȂ�Entity�ւ̃|�C���^�B���s�����ꍇ��nullptr��Ԃ��܂��B</returns>
    Entity* Import(World& world);

    /// <summary>
    /// �t�@�C���p�X�𒼐ڎw�肵�ă��f���t�@�C����ǂݍ��ނ��߂̃��[�e�B���e�B�֐��ł��B
    /// </summary>
    /// <param name="path">�ǂݍ��ރ��f���t�@�C���ւ̃��C�h������p�X�B</param>
    /// <param name="world">�G���e�B�e�B��R���|�[�l���g���Ǘ�����World�N���X�̃C���X�^���X�B</param>
    /// <returns>���f���̃��[�g�ƂȂ�Entity�ւ̃|�C���^�B���s�����ꍇ��nullptr��Ԃ��܂��B</returns>
    Entity* Import(const std::wstring& path, World& world);

    // ���f���S�̂̃X�P�[����ݒ肵�܂��B
    void SetGlobalScale(float scale) { m_globalScale = scale; }
    float GetGlobalScale() const { return m_globalScale; }

    // �ڐ��Ə]�@�����v�Z���邩�ݒ肵�܂��B
    void SetCalculateTangents(bool enabled) { m_calculateTangents = enabled; }
    bool GetCalculateTangents() const { return m_calculateTangents; }

    // �@�������݂��Ȃ��ꍇ�ɐ������邩�ݒ肵�܂��B
    void SetGenerateNormals(bool enabled) { m_generateNormals = enabled; }
    bool GetGenerateNormals() const { return m_generateNormals; }

    // UV���W��Y���𔽓]�����邩�ݒ肵�܂��B
    void SetFlipUVs(bool enabled) { m_flipUVs = enabled; }
    bool GetFlipUVs() const { return m_flipUVs; }

    // ���꒸�_���������čœK�����邩�ݒ肵�܂��B
    void SetJoinIdenticalVertices(bool enabled) { m_joinIdenticalVertices = enabled; }
    bool GetJoinIdenticalVertices() const { return m_joinIdenticalVertices; }

    // �}�e���A�����C���|�[�g���邩�ݒ肵�܂��B
    void SetImportMaterials(bool enabled) { m_importMaterials = enabled; }
    bool GetImportMaterials() const { return m_importMaterials; }

private:
    // Assimp�̃m�[�h�c���[���ċA�I�ɑ������A���b�V���A�}�e���A���A�{�[����������
    void ProcessNode(aiNode* node, const aiScene* scene, World& world, Entity* parentEntity, const std::vector<Material*>& materials, Skeleton* skeleton);

    // �A�j���[�V�����f�[�^��ǂݍ��݁A�Ǝ��̃f�[�^�\���ɕϊ�
    void ProcessAnimations(const aiScene* scene, std::vector<Animation*>& animations);

    // �{�[���K�w��Assimp�̃m�[�h�c���[����ċA�I�ɓǂݍ���
    void ReadSkeletonHierarchy(Bone* parentBone, const aiNode* node);

    // �e���_�Ƀ{�[���E�F�C�g��ID�������[�h
    void LoadBones(std::vector<Mesh::Vertex>& vertices, aiMesh* mesh, Skeleton* skeleton);

    // �}�e���A���f�[�^�ƃe�N�X�`����ǂݍ��݁A�}�e���A���I�u�W�F�N�g�𐶐�
    Material* ProcessMaterial(aiMaterial* mat, const aiScene* scene, DescriptorAllocator* srvAllocator);

    // �C���|�[�g�������f���̊K�w�\�����f�o�b�O�p�ɃR���\�[���o��
    void PrintHierarchy(Transform* transform, int level);
};