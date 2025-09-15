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

Quaternion::Quaternion(const DirectX::XMVECTOR& xmquat)
{
    XMStoreFloat4((XMFLOAT4*)components, xmquat);
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

Quaternion Quaternion::Euler(float x, float y, float z)
{
    return XMQuaternionRotationRollPitchYaw(x * Mathf::Deg2Rad, y * Mathf::Deg2Rad, z * Mathf::Deg2Rad);
}


Quaternion Quaternion::Euler(const Vector3& euler)
{
    return XMQuaternionRotationRollPitchYaw(euler.x * Mathf::Deg2Rad, euler.y * Mathf::Deg2Rad, euler.z * Mathf::Deg2Rad);
}

Quaternion Quaternion::LookRotation(const Vector3& forward, const Vector3& upwards)
{
    const Vector3 zaxis = Vector3::Normalize(forward);
    const Vector3 xaxis = Vector3::Normalize(Vector3::Cross(upwards, forward));
    const Vector3 yaxis = Vector3::Cross(zaxis, xaxis);
    const DirectX::XMMATRIX mat = DirectX::XMMatrixSet
    (
        xaxis.x, xaxis.y, xaxis.z, 0.0f,
        yaxis.x, yaxis.y, yaxis.z, 0.0f,
        zaxis.x, zaxis.y, zaxis.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    return XMQuaternionRotationMatrix(mat);
}

Quaternion Quaternion::Slerp(const Quaternion& a, const Quaternion& b, float t)
{
    return XMQuaternionSlerp(a.ToXMVECTOR(), b.ToXMVECTOR(), Mathf::Clamp(t, 0.0f, 1.0f));
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

Vector3 operator*(const Quaternion& q, const Vector3& v)
{
    Quaternion v_quat(v.x, v.y, v.z, 0.0f);

    Quaternion q_inv(-q.x, -q.y, -q.z, q.w);

    Quaternion result_quat = q * v_quat * q_inv;

    return Vector3(result_quat.x, result_quat.y, result_quat.z);
}
