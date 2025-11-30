#pragma once

class Plane
{
private:
    // 平面の法線ベクトル(a,b,c)
    Vector3 m_normal;

    // 平面から法線に沿った原点までの距離(d)
    float m_distance;

public:
    // デフォルトコンストラクタ
    Plane() = default;

    // 平面を定義する法線ベクトルと距離を基に新しい平面を作成します。
    // 引数 inNormal は正規化されていることを想定しています。
    Plane(const Vector3& inNormal, float distance);

    // 平面を定義する「平面上の任意の3点」を基に新しい平面を作成します。
    // 3点が同一直線上にある場合の動作は未定義です。
    Plane(const Vector3& point0, const Vector3& point1, const Vector3& point2);

    // 3点の位置を設定して平面を設定します。
    // 上部から見て時計回りで設定されます。
    // 3点が同一直線上にある場合の動作は未定義です。
    void Set3Points(const Vector3& point0, const Vector3& point1, const Vector3& point2);

    // ベクトルの法線と位置により平面を設定します。
    // 引数 inNormal は正規化されていることを想定しています。
    void SetNormalAndPosition(const Vector3& inNormal, const Vector3& inPoint);

    // レイが平面と交差している場合は true を返し、引数enterにはレイの始点から平面までの距離が格納されます。
    // レイが平面と交差していない場合は false を返し、引数enterには 0 が格納されます。
    bool Raycast(const Ray& ray, float& enter) const;
};