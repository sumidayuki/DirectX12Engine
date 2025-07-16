#pragma once

// Vector3, Vector2, Bounds, GraphicsBuffer, Reference などのインクルードを想定
#include "Reference.h"
#include <vector>
#include <cfloat> // FLT_MAX, -FLT_MAX のために追加

/// <summary>
/// 3Dモデルのジオメトリデータを表します。
/// 頂点バッファとインデックスバッファへの参照を保持します。
/// </summary>
class Mesh : public Reference
{
public:
    /// <summary>
    /// 頂点のレイアウトを定義します。
    /// </summary>
    struct Vertex
    {
        Vector3 position; // 頂点座標 (x, y, z)
        Vector3 normal;   // 法線
        Vector2 uv;       // テクスチャ座標 (u, v)
        Vector3 tangent;
    };

private:
    ComPtr<GraphicsBuffer>  m_vertexBuffer;   // 頂点バッファ
    ComPtr<GraphicsBuffer>  m_indexBuffer;    // インデックスバッファ
    Bounds                  m_bounds;         // モデル空間での境界ボックス
    UINT                    m_indexCount;     // インデックス数
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
    /// 頂点データとインデックスデータからメッシュをセットアップします。
    /// </summary>
    void SetupMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
    {
        // 頂点バッファの作成
        m_vertexBuffer.Attach(new GraphicsBuffer(
            GraphicsBuffer::Target::Vertex,
            GraphicsBuffer::UsageFlags::None,
            (int)vertices.size(),
            sizeof(Vertex),
            vertices.data()
        ));

        // インデックスバッファの作成
        m_indexBuffer.Attach(new GraphicsBuffer(
            GraphicsBuffer::Target::Index,
            GraphicsBuffer::UsageFlags::None,
            (int)indices.size(),
            sizeof(uint32_t),
            indices.data()
        ));

        m_indexCount = (UINT)indices.size();

        // --- バウンディングボックスの計算を実装 ---
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

    // 以下、ゲッター/セッター関数は変更なし
    GraphicsBuffer* GetVertexBuffer() const { return m_vertexBuffer.Get(); }
    GraphicsBuffer* GetIndexBuffer() const { return m_indexBuffer.Get(); }
    UINT GetIndexCount() const { return m_indexCount; }
    const Bounds& GetBounds() const { return m_bounds; }
    void SetMaterialIndex(int index) { m_materialIndex = index; }
    int GetMaterialIndex() const { return m_materialIndex; }
};