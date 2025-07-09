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
};