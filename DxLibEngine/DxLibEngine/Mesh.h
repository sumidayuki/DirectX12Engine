#pragma once

#define MAX_BONE_INFLUENCE 4 // 1���_�����肪�e�����󂯂�{�[���̍ő吔

struct BoneWeights
{
    int boneIndex[MAX_BONE_INFLUENCE];
    float weight[MAX_BONE_INFLUENCE];
};

/// <summary>
/// 3D���f���̃W�I���g���f�[�^��\���܂��B
/// ���_�o�b�t�@�ƃC���f�b�N�X�o�b�t�@�ւ̎Q�Ƃ�ێ����܂��B
/// </summary>
class Mesh : public Reference
{
public:
    /// <summary>
    /// ���_�̃��C�A�E�g���`���܂��B
    /// </summary>
    struct Vertex
    {
        Vector3 position;   // ���_���W (x, y, z)
        Vector3 normal;     // �@��
        Vector2 uv;         // �e�N�X�`�����W (u, v)
        Vector3 tangent;    // �ڐ�
    };

private:
    ComPtr<GraphicsBuffer>  m_vertexBuffer;   // ���_�o�b�t�@
    ComPtr<GraphicsBuffer>  m_indexBuffer;    // �C���f�b�N�X�o�b�t�@
    Bounds                  m_bounds;         // ���f����Ԃł̋��E�{�b�N�X
    UINT                    m_indexCount;     // �C���f�b�N�X��
    int                     m_materialIndex;

public:
    Mesh();

    virtual ~Mesh() = default;

    /// <summary>
    /// ���_�f�[�^�ƃC���f�b�N�X�f�[�^���烁�b�V�����Z�b�g�A�b�v���܂��B
    /// </summary>
    void SetupMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

    GraphicsBuffer* GetVertexBuffer() const { return m_vertexBuffer.Get(); }
    GraphicsBuffer* GetIndexBuffer() const { return m_indexBuffer.Get(); }
    UINT GetIndexCount() const { return m_indexCount; }
    const Bounds& GetBounds() const { return m_bounds; }
    void SetMaterialIndex(int index) { m_materialIndex = index; }
    int GetMaterialIndex() const { return m_materialIndex; }
};