#pragma once

#define MAX_BONE_INFLUENCE 4 // 1���_�����肪�e�����󂯂�{�[���̍ő吔

struct SubMesh
{
    UINT startIndex;
    UINT indexCount;
    UINT materialIndex;
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
        Vector3 position;                                               // ���_���W (x, y, z)
        Vector3 normal;                                                 // �@��
        Vector2 uv;                                                     // �e�N�X�`�����W (u, v)
        Vector3 tangent;                                                // �ڐ�
        int boneIDs[MAX_BONE_INFLUENCE] = { -1, -1, -1, -1 };           // �{�[��ID
        float weights[MAX_BONE_INFLUENCE] = { 0.0f, 0.0f, 0.0f, 0.0f }; // �E�F�C�g
    };

private:
    ComPtr<GraphicsBuffer>  m_vertexBuffer;   // ���_�o�b�t�@
    ComPtr<GraphicsBuffer>  m_indexBuffer;    // �C���f�b�N�X�o�b�t�@
    Bounds                  m_bounds;         // ���f����Ԃł̋��E�{�b�N�X
    std::vector<SubMesh>    m_subMeshes;      // �T�u���b�V���̃��X�g

    // �ꎞ�I�Ȓ��_/�C���f�b�N�X�f�[�^��ێ�����
    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_indices;

public:
    Mesh();

    virtual ~Mesh() = default;

    /// <summary>
    /// ���_�f�[�^�ƃC���f�b�N�X�f�[�^���烁�b�V�����Z�b�g�A�b�v���܂��B
    /// ModelImporter�Ńf�[�^���W�񂵂���Ɉ�x�����Ăяo���܂��B
    /// </summary>
    void SetupMesh();

    void AddVertex(const Vertex& vertex) { m_vertices.push_back(vertex); }
    void AddIndex(uint32_t index) { m_indices.push_back(index); }
    void AddVertices(const std::vector<Vertex>& vertices) { m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end()); }
    void AddIndices(const std::vector<uint32_t>& indices) { m_indices.insert(m_indices.end(), indices.begin(), indices.end()); }
    void AddSubMesh(UINT startIndex, UINT indexCount, UINT materialIndex);

    GraphicsBuffer* GetVertexBuffer() const { return m_vertexBuffer.Get(); }
    GraphicsBuffer* GetIndexBuffer() const { return m_indexBuffer.Get(); }
    UINT GetTotalIndexCount() const;
    UINT GetVertexCount() const { return (UINT)m_vertices.size(); }
    const Bounds& GetBounds() const { return m_bounds; }
    void SetBounds(const Bounds& bounds) { m_bounds = bounds; }

    // �T�u���b�V���̐��Ə����擾����֐�
    UINT GetSubMeshCount() const { return (UINT)m_subMeshes.size(); }
    const SubMesh& GetSubMesh(UINT index) const { return m_subMeshes[index]; }
};