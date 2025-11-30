#include "Plane.h"

Plane::Plane(const Vector3& inNormal, float distance)
    : m_normal(inNormal)
    , m_distance(distance)
{

}

Plane::Plane(const Vector3& point0, const Vector3& point1, const Vector3& point2)
{
    Set3Points(point0, point1, point2);
}

void Plane::Set3Points(const Vector3& point0, const Vector3& point1, const Vector3& point2)
{
    // point0 から point1 へ向かうベクトル
    const Vector3 v01 = point1 - point0;

    // point0 から point2 へ向かうベクトル
    const Vector3 v02 = point2 - point0;

#ifdef _DEBUG
    // 3点が同一直線上にある場合はエラー
    if (Mathf::Approximately(Vector3::Cross(v01, v02).Magnitude(), 0.0f))
    {
        assert(0);
    }
#endif

    // v01とv02の両方に直交するベクトルを外積で求める。
    const Vector3 orthogonal = Vector3::Cross(v01, v02);

    // orthogonalを正規化したものが法線ベクトル。
    SetNormalAndPosition(orthogonal.Normalized(), point0);
}

void Plane::SetNormalAndPosition(const Vector3& inNormal, const Vector3& inPoint)
{
    m_normal = inNormal;

    //----------------------------------------------------------------------------------------------------------------------------------------------------
    // 平面方程式 ax + by + cz + d = 0 より、
    // 
    //  d = -(ax + by + cz)
    //    = -((a,b,c)・(x,y,z))
    //    = -(inNormal・inPoint)
    //----------------------------------------------------------------------------------------------------------------------------------------------------
    m_distance = -Vector3::Dot(m_normal, inPoint);
}

bool Plane::Raycast(const Ray& ray, float& enter) const
{
    // 分母を計算する
    const float denominator = Vector3::Dot(m_normal, ray.GetDirection());

    // 分母が 0 になる場合は平面と光線が平行なので、
    // 交点は 0個 または 無数に存在する。
    if (Mathf::Approximately(denominator, 0.0f))
    {
        enter = 0.0f;
        return false;
    }

    // 分子を計算する
    const float numerator = -Vector3::Dot(m_normal, ray.GetOrigin()) - m_distance;

    // t を求める
    enter = numerator / denominator;

    // 交点がレイの前方にある場合だけ有効。
    return enter > 0.0f;
}
