#pragma once

// minマクロが定義済みだった場合は無効化する
#ifdef min
#undef min
#endif

// maxマクロが定義済みだった場合は無効化する
#ifdef max
#undef max
#endif

// XYZ軸に平行な境界ボックス (AABB = Axis Aligned Bounding Box)
class Bounds
{
private:
    // 中心座標
    Vector3 m_center;

    // 辺の半分の長さ
    Vector3 m_extents;

public:
    // デフォルトコンストラクタ
    Bounds() = default;

    // コンストラクタ
    Bounds(const Vector3& center, const Vector3& size);

    // 中心座標を設定します。
    void SetCenter(const Vector3& center) { m_center = center; }

    // 中心座標をを取得します。
    const Vector3& GetCenter() const { return m_center; }

    // バウンディングサイズを設定します。
    void SetSize(const Vector3& size) { m_extents = size * 0.5f; }

    // バウンディングサイズを取得します。
    Vector3 GetSize() const { return m_extents * 2.0f; }

    // バウンディングサイズの半分を設定します。
    void SetExtents(const Vector3& extents) { m_extents = extents; }

    // バウンディングサイズの半分を取得します。
    const Vector3& GetExtents() const { return m_extents; }

    // バウンディングボックスの最小値を設定します。
    void SetMin(const Vector3& min) { SetMinMax(min, GetMax()); }

    // バウンディングボックスの最小値を取得します。
    // 常にcenter変数からextents変数を引いた値となります。
    Vector3 GetMin() const { return m_center - m_extents; }

    // バウンディングボックスの最大値を設定します。
    void SetMax(const Vector3& max) { SetMinMax(GetMin(), max); }

    // バウンディングボックスの最大値を取得します。
    // 常にcenter変数とextents変数を足した値となります。
    Vector3 GetMax() const { return m_center + m_extents; }

    // バウンディングボックス上の最も近い点を取得します。
    // 引数 point がバウンディングボックス内部にある場合はそのまま point を返します。
    Vector3 ClosestPoint(const Vector3& point) const;

    // 引数 point がバウンディングボックス内部に含まれている場合は true を返します。
    bool Contains(const Vector3& point) const;

    // 指定した amount によって size を大きくします。
    void Expand(float amount);

    // このバウンディングボックスが引数 other と交差している場合は true を返します。
    bool Intersects(const Bounds& other) const;

    // 最小値と最大値を指定してバウンディングボックスを設定します。
    void SetMinMax(const Vector3& min, const Vector3& max);

    // 引数 point との最小距離を2乗した値を返します。
    float SqrDistance(const Vector3& point) const;

    // このバウンディングボックスが引数 other と等しい場合は true を返します。
    bool operator==(const Bounds& other);

    // このバウンディングボックスが引数 other と等しくない場合は true を返します。
    bool operator!=(const Bounds& other);
};

