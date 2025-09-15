#include "Mesh.h"
#include <algorithm> // std::min, std::max

Mesh::Mesh()
    : m_vertexBuffer(nullptr)
    , m_indexBuffer(nullptr)
{
}

void Mesh::SetupMesh()
{
    // 頂点バッファの作成
    m_vertexBuffer.Attach(new GraphicsBuffer(
        GraphicsBuffer::Target::Vertex,
        GraphicsBuffer::UsageFlags::None,
        (int)m_vertices.size(),
        sizeof(Vertex),
        m_vertices.data()
    ));

    // インデックスバッファの作成
    m_indexBuffer.Attach(new GraphicsBuffer(
        GraphicsBuffer::Target::Index,
        GraphicsBuffer::UsageFlags::None,
        (int)m_indices.size(),
        sizeof(uint32_t),
        m_indices.data()
    ));

    // バウンディングボックスの計算を実装
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
        m_bounds.SetMinMax(minPoint, maxPoint);
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