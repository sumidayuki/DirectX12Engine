#pragma once

// Vector3, Vector2, Bounds, GraphicsBuffer, Reference �Ȃǂ̃C���N���[�h��z��
#include "Reference.h"
#include <vector>
#include <cfloat> // FLT_MAX, -FLT_MAX �̂��߂ɒǉ�

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
        Vector3 position; // ���_���W (x, y, z)
        Vector3 normal;   // �@��
        Vector2 uv;       // �e�N�X�`�����W (u, v)
        Vector3 tangent;
    };

private:
    ComPtr<GraphicsBuffer>  m_vertexBuffer;   // ���_�o�b�t�@
    ComPtr<GraphicsBuffer>  m_indexBuffer;    // �C���f�b�N�X�o�b�t�@
    Bounds                  m_bounds;         // ���f����Ԃł̋��E�{�b�N�X
    UINT                    m_indexCount;     // �C���f�b�N�X��
    int                     m_materialIndex;

public:
    Mesh()
        : m_vertexBuffer(nullptr)
        , m_indexBuffer(nullptr)
        , m_indexCount(0)
        , m_materialIndex(0)
    {
    }

    virtual ~Mesh() = default;

    /// <summary>
    /// ���_�f�[�^�ƃC���f�b�N�X�f�[�^���烁�b�V�����Z�b�g�A�b�v���܂��B
    /// </summary>
    void SetupMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
    {
        // ���_�o�b�t�@�̍쐬
        m_vertexBuffer.Attach(new GraphicsBuffer(
            GraphicsBuffer::Target::Vertex,
            GraphicsBuffer::UsageFlags::None,
            (int)vertices.size(),
            sizeof(Vertex),
            vertices.data()
        ));

        // �C���f�b�N�X�o�b�t�@�̍쐬
        m_indexBuffer.Attach(new GraphicsBuffer(
            GraphicsBuffer::Target::Index,
            GraphicsBuffer::UsageFlags::None,
            (int)indices.size(),
            sizeof(uint32_t),
            indices.data()
        ));

        m_indexCount = (UINT)indices.size();

        // --- �o�E���f�B���O�{�b�N�X�̌v�Z������ ---
        if (!vertices.empty())
        {
            Vector3 minPoint(FLT_MAX, FLT_MAX, FLT_MAX);
            Vector3 maxPoint(-FLT_MAX, -FLT_MAX, -FLT_MAX);

            for (const auto& vertex : vertices)
            {
                minPoint.x = std::min(minPoint.x, vertex.position.x);
                minPoint.y = std::min(minPoint.y, vertex.position.y);
                minPoint.z = std::min(minPoint.z, vertex.position.z);

                maxPoint.x = std::max(maxPoint.x, vertex.position.x);
                maxPoint.y = std::max(maxPoint.y, vertex.position.y);
                maxPoint.z = std::max(maxPoint.z, vertex.position.z);
            }
            m_bounds.SetMinMax(minPoint, maxPoint);
        }
    }

    // �ȉ��A�Q�b�^�[/�Z�b�^�[�֐��͕ύX�Ȃ�
    GraphicsBuffer* GetVertexBuffer() const { return m_vertexBuffer.Get(); }
    GraphicsBuffer* GetIndexBuffer() const { return m_indexBuffer.Get(); }
    UINT GetIndexCount() const { return m_indexCount; }
    const Bounds& GetBounds() const { return m_bounds; }
    void SetMaterialIndex(int index) { m_materialIndex = index; }
    int GetMaterialIndex() const { return m_materialIndex; }
};