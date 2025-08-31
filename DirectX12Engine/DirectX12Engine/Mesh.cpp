#include "Mesh.h"
#include <algorithm> // std::min, std::max

Mesh::Mesh()
    : m_vertexBuffer(nullptr)
    , m_indexBuffer(nullptr)
{
}

void Mesh::SetupMesh()
{
    // ���_�o�b�t�@�̍쐬
    m_vertexBuffer.Attach(new GraphicsBuffer(
        GraphicsBuffer::Target::Vertex,
        GraphicsBuffer::UsageFlags::None,
        (int)m_vertices.size(),
        sizeof(Vertex),
        m_vertices.data()
    ));

    // �C���f�b�N�X�o�b�t�@�̍쐬
    m_indexBuffer.Attach(new GraphicsBuffer(
        GraphicsBuffer::Target::Index,
        GraphicsBuffer::UsageFlags::None,
        (int)m_indices.size(),
        sizeof(uint32_t),
        m_indices.data()
    ));

    // �o�E���f�B���O�{�b�N�X�̌v�Z������
    if (!m_vertices.empty())
    {
        Vector3 minPoint(FLT_MAX, FLT_MAX, FLT_MAX);
        Vector3 maxPoint(-FLT_MAX, -FLT_MAX, -FLT_MAX);

        for (const auto& vertex : m_vertices)
        {
            minPoint.x = std::min(minPoint.x, vertex.position.x);
            minPoint.y = std::min(minPoint.y, vertex.position.y);
            minPoint.z = std::min(minPoint.z, vertex.position.z);
            maxPoint.x = std::max(maxPoint.x, vertex.position.x);
            maxPoint.y = std::max(maxPoint.y, vertex.position.y);
            maxPoint.z = std::max(maxPoint.z, vertex.position.z);
        }
        m_bounds.SetCenter((minPoint + maxPoint) * 0.5f);
        m_bounds.SetExtents((maxPoint - minPoint) * 0.5f);
    }
}

void Mesh::AddSubMesh(UINT startIndex, UINT indexCount, UINT materialIndex)
{
    m_subMeshes.push_back({ startIndex, indexCount, materialIndex });
}

UINT Mesh::GetTotalIndexCount() const
{
    UINT totalCount = 0;
    for (const auto& subMesh : m_subMeshes)
    {
        totalCount += subMesh.indexCount;
    }
    return totalCount;
}