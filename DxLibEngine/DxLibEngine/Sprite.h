#pragma once

class Sprite : public Reference
{
private:
    // フレンド
    friend class SpriteRendererSystem;

    // 各頂点は位置(x,y)とテクスチャ座標(u,v)を持つ。
    struct Vertex
    {
        Vector2 position;
        Vector2 uv;
    };

private:
    std::string             m_name;                 // テクスチャ名
    ComPtr<Texture2D>       m_texture;              // 適用するテクスチャ
    Rect                    m_rect;                 // テクスチャ矩形
    Vector2                 m_pivot;                // ピボット
    float                   m_pixelsPerUnitX;       // 1単位あたりのピクセル数
    float                   m_pixelsPerUnitY;       // 1単位あたりのピクセル数
    Bounds                  m_bounds;               // モデル空間内での境界ボックス
    std::vector<Vector2>    m_vertices;             // 頂点位置(x,y)の配列
    std::vector<Vector2>    m_uv;                   // テクスチャ座標(u,v)の配列
    std::vector<uint16_t>   m_triangles;            // インデックスの配列
    ComPtr<GraphicsBuffer>  m_vertexBuffer;         // 頂点バッファ
    ComPtr<GraphicsBuffer>  m_indexBuffer;          // インデックスバッファ
    Rect                    m_textureRect;          // Tight指定時に補正されたテクスチャ矩形 (FillRectの場合はrectと同等、Unityは例外を発生させる)
    Vector2                 m_textureRectOffset;    // 
    Vector2                 m_minUV;                // 最小UV値
    Vector2                 m_maxUV;                // 最大UV値

private:
    // コンストラクタ
    Sprite(Texture2D* texture, const Rect& rect, const Vector2& pivot, float pixelsPerUnitX = 1.0f, float pixelsPerUnitY = 1.0f);

    // 仮想デストラクタ
    ~Sprite() = default;

    /// <summary>
    /// 全ての頂点を包含する矩形の最小値と最大値を求めます。
    /// </summary>
    /// <param name="vertices"></param>
    /// <param name="vertexCount"></param>
    /// <param name="minPoint"></param>
    /// <param name="maxPoint"></param>
    static void CalcBoundingRect(const Vector2 vertices[], int vertexCount, Vector2& minPoint, Vector2& maxPoint);

    // 頂点バッファを取得します。
    GraphicsBuffer* GetVertexBuffer() const { return m_vertexBuffer.Get(); }

    // インデックスバッファを取得します。
    GraphicsBuffer* GetIndexBuffer() const { return m_indexBuffer.Get(); }

public:
    /// <summary>
    /// スプライトを1つ生成します。
    /// </summary>
    /// <param name="texture"></param>
    /// <param name="rect"></param>
    /// <param name="pivot"></param>
    /// <param name="pixelsPerUnitX"></param>
    /// <param name="pixelsPerUnitY"></param>
    /// <returns></returns>
    static Sprite* Create(Texture2D* texture, const Rect& rect, const Vector2& pivot, float pixelsPerUnitX, float pixelsPerUnitY);

    // 関連付けられたテクスチャを取得します。
    Texture2D* GetTexture() const { return m_texture.Get(); }

    // テクスチャ矩形を取得します。
    const Rect& GetRect() const { return m_rect; }

    // ピボットを取得します。
    const Vector2& GetPivot() const { return m_pivot; }

    // 1単位あたりのピクセル数を取得します。
    float GetPixelsPerUnitX() const { return m_pixelsPerUnitX; }

    // 1単位あたりのピクセル数を取得します。
    float GetPixelsPerUnitY() const { return m_pixelsPerUnitY; }

    // モデル空間内での境界ボックスを取得します。
    const Bounds& GetBounds() const { return m_bounds; }

    // 頂点位置(x,y)の配列を取得します。
    const std::vector<Vector2>& GetVertices() const { return m_vertices; }

    // テクスチャ座標(u,v)の配列
    const std::vector<Vector2>& GetUV() const { return m_uv; }

    // インデックスの配列
    const std::vector<uint16_t>& GetTriangles() const { return m_triangles; }

    /// <summary>
    /// 内部データを破棄し、新しいジオメトリデータを基にしてスプライトを再作成します。
    /// </summary>
    /// <param name="vertices"></param>
    /// <param name="triangles"></param>
    void OverrideGeometry(const std::vector<Vector2>& vertices, const std::vector<uint16_t>& triangles);
};

