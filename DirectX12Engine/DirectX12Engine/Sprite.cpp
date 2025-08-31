#include "Sprite.h"

Sprite::Sprite(Texture2D* texture, const Rect& rect, const Vector2& pivot, float pixelsPerUnitX, float pixelsPerUnitY)
    : m_texture(texture)
    , m_rect(rect)
    , m_pivot(pivot)
    , m_pixelsPerUnitX(pixelsPerUnitX)
    , m_pixelsPerUnitY(pixelsPerUnitY)
    , m_bounds(Vector3::zero, Vector3::zero)
    , m_textureRectOffset(Vector2::zero)
{
}


Sprite* Sprite::Create(Texture2D* texture, const Rect& rect, const Vector2& pivot, float pixelsPerUnitX, float pixelsPerUnitY)
{
    // 引数の妥当性チェック
    assert(texture != nullptr);
    assert(rect.width <= texture->GetWidth());
    assert(rect.height <= texture->GetHeight());
    assert(pixelsPerUnitX > 0.0f);
    assert(pixelsPerUnitY > 0.0f);

    // モデル空間内でのスプライトサイズ(横幅と高さ)を計算する
    Vector2 spriteSize;
    spriteSize.x = rect.width / pixelsPerUnitX;
    spriteSize.y = rect.height / pixelsPerUnitY;

    // スプライトの左下隅からピボット位置までのオフセットを計算する
    Vector2 pivotOffset;
    pivotOffset.x = spriteSize.x * pivot.x;
    pivotOffset.y = spriteSize.y * pivot.y;
    
    const std::vector<Vector2> vertices
    {
        Vector2(0.0f - pivotOffset.x,         0.0f - pivotOffset.y), // 左下の頂点
        Vector2(0.0f - pivotOffset.x, spriteSize.y - pivotOffset.y), // 左上の頂点
        Vector2(spriteSize.x - pivotOffset.x,         0.0f - pivotOffset.y), // 右下の頂点
        Vector2(spriteSize.x - pivotOffset.x, spriteSize.y - pivotOffset.y), // 右上の頂点
    };

    // インデックス配列の作成
    const std::vector<uint16_t> triangles
    {
        0, 1, 2,    // 左下の三角形
        2, 1, 3,    // 右上の三角形
    };

    // スプライトの作成
    Sprite* sprite = new Sprite(texture, rect, pivot, pixelsPerUnitX, pixelsPerUnitY);
    sprite->OverrideGeometry(vertices, triangles);
    return sprite;
}


void Sprite::CalcBoundingRect(const Vector2 vertices[], int vertexCount, Vector2& minPoint, Vector2& maxPoint)
{
    minPoint.x = Mathf::Infinity;
    minPoint.y = Mathf::Infinity;
    maxPoint.x = Mathf::NegativeInfinity;
    maxPoint.y = Mathf::NegativeInfinity;

    for (int i = 0; i < vertexCount; i++)
    {
        // 一旦ローカル変数に格納する
        const float x = vertices[i].x;
        const float y = vertices[i].y;

        // X軸方向の最小値と最大値を更新していく
        if (x < minPoint.x)
        {
            minPoint.x = x;
        }
        else if (x > maxPoint.x)
        {
            maxPoint.x = x;
        }

        // Y軸方向の最小値と最大値を更新していく
        if (y < minPoint.y)
        {
            minPoint.y = y;
        }
        else if (y > maxPoint.y)
        {
            maxPoint.y = y;
        }
    }
}


void Sprite::OverrideGeometry(const std::vector<Vector2>& vertices, const std::vector<uint16_t>& triangles)
{
    // グラフィックスバッファを破棄する
    m_vertexBuffer = nullptr;
    m_indexBuffer = nullptr;

    // メンバ変数にコピーしておく
    m_vertices = vertices;
    m_triangles = triangles;

    // 頂点数
    const int vertexCount = (int)vertices.size();

    // 全ての頂点を包含する矩形の最小値と最大値を求める。
    Vector2 minPoint;
    Vector2 maxPoint;
    CalcBoundingRect(&m_vertices[0], vertexCount, minPoint, maxPoint);

    // 境界ボックスが決定する
    m_bounds.SetMinMax(Vector3(minPoint, 0.0f), Vector3(maxPoint, 0.0f));

    // 左下と右上のUV座標を求める
    Vector2 minUV;
    minUV.x = m_rect.x / m_texture->GetWidth();
    minUV.y = m_rect.y / m_texture->GetHeight();

    Vector2 maxUV;
    maxUV.x = (m_rect.x + m_rect.width) / m_texture->GetWidth();
    maxUV.y = (m_rect.y + m_rect.height) / m_texture->GetHeight();

    // スプライトサイズ(横幅と高さ)
    Vector2 spriteSize;
    spriteSize.x = maxPoint.x - minPoint.x;
    spriteSize.y = maxPoint.y - minPoint.y;

    // テクスチャ座標(u,v)の配列を作成する
    m_uv.resize(vertexCount);
    m_minUV = Vector2::positiveInfinity;
    m_maxUV = Vector2::negativeInfinity;
    for (int i = 0; i < vertexCount; i++)
    {
        // 矩形左下隅から対象の頂点までのオフセット
        Vector2 offset;
        offset.x = m_vertices[i].x - minPoint.x;
        offset.y = m_vertices[i].y - minPoint.y;

        // スプライトサイズ(横幅と高さ)をそれぞれ1.0とした時のオフセットの比率
        Vector2 ratio;
        ratio.x = offset.x / spriteSize.x;
        ratio.y = offset.y / spriteSize.y;

        // テクスチャ座標(u,v)
        m_uv[i].x = minUV.x + (maxUV.x - minUV.x) * ratio.x;
        m_uv[i].y = minUV.y + (maxUV.y - minUV.y) * ratio.y;

        if (m_uv[i].x < m_minUV.x)
        {
            m_minUV.x = m_uv[i].x;
        }
        else if (m_uv[i].x > m_maxUV.x)
        {
            m_maxUV.x = m_uv[i].x;
        }

        if (m_uv[i].y < m_minUV.y)
        {
            m_minUV.y = m_uv[i].y;
        }
        else if (m_uv[i].y > m_maxUV.y)
        {
            m_maxUV.y = m_uv[i].y;
        }
    }

    //----------------------------------------------------------------------------------------------------------------------------------------------------
    // 頂点バッファの作成 (ビデオメモリ上に頂点データを置きたい)
    //----------------------------------------------------------------------------------------------------------------------------------------------------
    std::vector<Vertex> initialData(vertexCount);
    for (int i = 0; i < vertexCount; i++)
    {
        initialData[i].position = m_vertices[i];     // 位置(x,y)をコピー
        initialData[i].uv = m_uv[i];                 // テクスチャ座標(u,v)をコピー
    }
    m_vertexBuffer.Attach(new GraphicsBuffer(GraphicsBuffer::Target::Vertex, GraphicsBuffer::UsageFlags::None, vertexCount, sizeof(Vertex), &initialData[0]));

    // インデックス配列を用意する (一旦システムメモリ上にインデックスデータを用意しておく)

    // インデックス数
    const int indexCount = (int)m_triangles.size();

    // インデックスバッファの作成
    m_indexBuffer.Attach(new GraphicsBuffer(GraphicsBuffer::Target::Index, GraphicsBuffer::UsageFlags::None, indexCount, sizeof(uint16_t), &m_triangles[0]));
}