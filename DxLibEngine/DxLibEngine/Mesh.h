#pragma once

#include "Reference.h"
#include "GraphicsBuffer.h"
#include "Bounds.h"

/// <summary>
/// 3Dモデルのジオメトリデータを表します。
/// 頂点バッファとインデックスバッファへの参照を保持します。
/// </summary>
class Mesh : public Reference
{
public:
    /// <summary>
    /// 頂点のレイアウトを定義します。
    /// 必要に応じて法線、UV座標、カラーなどを追加できます。
    /// </summary>
    struct Vertex
    {
        Vector3 position; // 頂点座標 (x, y, z)
        Vector3 normal;   // 法線
        Vector2 uv;       // テクスチャ座標 (u, v)
    };

private:
    ComPtr<GraphicsBuffer>  m_vertexBuffer;   // 頂点バッファ
    ComPtr<GraphicsBuffer>  m_indexBuffer;    // インデックスバッファ
    Bounds                  m_bounds;         // モデル空間での境界ボックス
    UINT                    m_indexCount;     // インデックス数

public:
    // コンストラクタ
    Mesh()
        : m_vertexBuffer(nullptr)
        , m_indexBuffer(nullptr)
        , m_indexCount(0)
    {
    }

    // 仮想デストラクタ
    virtual ~Mesh() = default;

    /// <summary>
    /// 頂点データとインデックスデータからメッシュをセットアップします。
    /// </summary>
    /// <param name="vertices">頂点データの配列</param>
    /// <param name="indices">インデックスデータの配列</param>
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
            sizeof(uint32_t), // 3Dモデルは頂点数が多いため32bitインデックスを使用
            indices.data()
        ));

        m_indexCount = (UINT)indices.size();

        // TODO: 頂点データからバウンディングボックスを計算する処理
    }

    /// <summary>
    /// 頂点バッファを取得します。
    /// </summary>
    /// <returns>頂点バッファのポインタ</returns>
    GraphicsBuffer* GetVertexBuffer() const { return m_vertexBuffer.Get(); }

    /// <summary>
    /// インデックスバッファを取得します。
    /// </summary>
    /// <returns>インデックスバッファのポインタ</returns>
    GraphicsBuffer* GetIndexBuffer() const { return m_indexBuffer.Get(); }

    /// <summary>
    /// インデックス数を取得します。
    /// </summary>
    /// <returns>インデックス数</returns>
    UINT GetIndexCount() const { return m_indexCount; }

    /// <summary>
    /// 境界ボックスを取得します。
    /// </summary>
    /// <returns>境界ボックス</returns>
    const Bounds& GetBounds() const { return m_bounds; }
};