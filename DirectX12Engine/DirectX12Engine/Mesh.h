#pragma once

#define MAX_BONE_INFLUENCE 4 // 1頂点あたりが影響を受けるボーンの最大数

struct SubMesh
{
    UINT startIndex;
    UINT indexCount;
    UINT materialIndex;
};

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
        Vector3 position;                                               // 頂点座標 (x, y, z)
        Vector3 normal;                                                 // 法線
        Vector2 uv;                                                     // テクスチャ座標 (u, v)
        Vector3 tangent;                                                // 接線
        int boneIDs[MAX_BONE_INFLUENCE] = { -1, -1, -1, -1 };           // ボーンID
        float weights[MAX_BONE_INFLUENCE] = { 0.0f, 0.0f, 0.0f, 0.0f }; // ウェイト
    };

private:
    ComPtr<GraphicsBuffer>  m_vertexBuffer;   // 頂点バッファ
    ComPtr<GraphicsBuffer>  m_indexBuffer;    // インデックスバッファ
    Bounds                  m_bounds;         // モデル空間での境界ボックス
    std::vector<SubMesh>    m_subMeshes;      // サブメッシュのリスト
    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_indices;

public:
    Mesh();

    virtual ~Mesh() = default;

    /// <summary>
    /// 頂点データとインデックスデータをビデオメモリ上に公開します。
    /// </summary>
    void SetupMesh();

    void AddVertex(const Vertex& vertex) { m_vertices.push_back(vertex); }
    void AddIndex(uint32_t index) { m_indices.push_back(index); }
    void AddVertices(const std::vector<Vertex>& vertices) { m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end()); }
    void SetVertices(const std::vector<Vertex>&& vertices);
    void AddIndices(const std::vector<uint32_t>& indices) { m_indices.insert(m_indices.end(), indices.begin(), indices.end()); }
	void SetIndices(const std::vector<uint32_t>&& indices, int subMeshIndex, bool calculateBounds = true, int baseVertex = 0);
    void AddSubMesh(UINT startIndex, UINT indexCount, UINT materialIndex);

    GraphicsBuffer* GetVertexBuffer() const { return m_vertexBuffer.Get(); }
    GraphicsBuffer* GetIndexBuffer() const { return m_indexBuffer.Get(); }
    const std::vector<uint32_t>& GetIndices() const { return m_indices; }
    std::vector<Vertex>& GetMutableVertices() { return m_vertices; }
    UINT GetTotalIndexCount() const;
    UINT GetVertexCount() const { return (UINT)m_vertices.size(); }
    const Bounds& GetBounds() const { return m_bounds; }
    void SetBounds(const Bounds& bounds) { m_bounds = bounds; }

    // サブメッシュの数と情報を取得する関数
    UINT GetSubMeshCount() const { return (UINT)m_subMeshes.size(); }
    const SubMesh& GetSubMesh(UINT index) const { return m_subMeshes[index]; }
};