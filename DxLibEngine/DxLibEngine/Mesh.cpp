#include "Mesh.h"

Mesh::Mesh()
    : m_vertexBuffer(nullptr)
    , m_indexBuffer(nullptr)
    , m_indexCount(0)
    , m_materialIndex(0)
{
}

void Mesh::SetupMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
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

    // �o�E���f�B���O�{�b�N�X�̌v�Z������
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
