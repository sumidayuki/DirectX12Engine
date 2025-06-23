#include "Quaternion.h"

using namespace DirectX;

const Quaternion Quaternion::identity(0, 0, 0, 1);

Quaternion::Quaternion(float x, float y, float z, float w)
    : x(x)
    , y(y)
    , z(z)
    , w(w)
{
}

DirectX::XMVECTOR Quaternion::ToXMVECTOR() const
{
    return DirectX::XMVectorSet(x, y, z, w);
}

Quaternion Quaternion::AngleAxis(float angle, const Vector3& axis)
{
    // É∆/2
    const float halfAngleInDegrees = angle * Mathf::Deg2Rad * 0.5f;

    // sin(É∆/2)Ç∆cos(É∆/2)
    const float s = Mathf::Sin(halfAngleInDegrees);
    const float c = Mathf::Cos(halfAngleInDegrees);

    const Vector3 normalizedAxis = axis.Normalized();

    Quaternion result;
    result.x = normalizedAxis.x * s;
    result.y = normalizedAxis.y * s;
    result.z = normalizedAxis.z * s;
    result.w = c;
    return result;
}

Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs) noexcept
{
#if 1
    return Quaternion
    (
        lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y,
        lhs.w * rhs.y + lhs.y * rhs.w + lhs.z * rhs.x - lhs.x * rhs.z,
        lhs.w * rhs.z + lhs.z * rhs.w + lhs.x * rhs.y - lhs.y * rhs.x,
        lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z
    );
#else
    return XMQuaternionMultiply(lhs.ToXMVECTOR(), rhs.ToXMVECTOR());
#endif
}
