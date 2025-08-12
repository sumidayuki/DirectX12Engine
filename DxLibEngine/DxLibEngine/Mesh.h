#pragma once

#define MAX_BONE_INFLUENCE 4 // 1頂点あたりが影響を受けるボーンの最大数

struct BoneWeights
{
    int boneIndex[MAX_BONE_INFLUENCE];
    float weight[MAX_BONE_INFLUENCE];
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
        Vector3 position;   // 頂点座標 (x, y, z)
        Vector3 normal;     // 法線
        Vector2 uv;         // テクスチャ座標 (u, v)
        Vector3 tangent;    // 接線
    };

private:
    ComPtr<GraphicsBuffer>  m_vertexBuffer;   // 頂点バッファ
    ComPtr<GraphicsBuffer>  m_indexBuffer;    // インデックスバッファ
    Bounds                  m_bounds;         // モデル空間での境界ボックス
    UINT                    m_indexCount;     // インデックス数
    int                     m_materialIndex;

public:
    Mesh();

    virtual ~Mesh() = default;

    /// <summary>
    /// 頂点データとインデックスデータからメッシュをセットアップします。
    /// </summary>
    void SetupMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

    GraphicsBuffer* GetVertexBuffer() const { return m_vertexBuffer.Get(); }
    GraphicsBuffer* GetIndexBuffer() const { return m_indexBuffer.Get(); }
    UINT GetIndexCount() const { return m_indexCount; }
    const Bounds& GetBounds() const { return m_bounds; }
    void SetMaterialIndex(int index) { m_materialIndex = index; }
    int GetMaterialIndex() const { return m_materialIndex; }
};