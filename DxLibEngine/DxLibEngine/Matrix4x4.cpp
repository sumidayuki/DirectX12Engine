#include "Matrix4x4.h"

using namespace DirectX;

const Matrix4x4 Matrix4x4::zero
(
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0
);


const Matrix4x4 Matrix4x4::identity
(
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
);

Matrix4x4::Matrix4x4(const float m[4][4])
    : _11(m[0][0]), _12(m[0][1]), _13(m[0][2]), _14(m[0][3])
    , _21(m[1][0]), _22(m[1][1]), _23(m[1][2]), _24(m[1][3])
    , _31(m[2][0]), _32(m[2][1]), _33(m[2][2]), _34(m[2][3])
    , _41(m[3][0]), _42(m[3][1]), _43(m[3][2]), _44(m[3][3])
{
}

Matrix4x4::Matrix4x4(float _11, float _12, float _13, float _14, float _21, float _22, float _23, float _24, float _31, float _32, float _33, float _34, float _41, float _42, float _43, float _44)
    : _11(_11), _12(_12), _13(_13), _14(_14)
    , _21(_21), _22(_22), _23(_23), _24(_24)
    , _31(_31), _32(_32), _33(_33), _34(_34)
    , _41(_41), _42(_42), _43(_43), _44(_44)
{
}

Matrix4x4::Matrix4x4(const DirectX::XMMATRIX& xmmatrix)
{
    XMStoreFloat4x4((XMFLOAT4X4*)m, xmmatrix);
}

DirectX::XMMATRIX Matrix4x4::ToXMMATRIX() const
{
    return XMLoadFloat4x4((const XMFLOAT4X4*)m);
}

void Matrix4x4::SetRow(int index, const Vector4& row)
{
    m[index][0] = row.x;
    m[index][1] = row.y;
    m[index][2] = row.z;
    m[index][3] = row.w;
}

void Matrix4x4::SetColumn(int index, const Vector4& column)
{
    m[0][index] = column.x;
    m[1][index] = column.y;
    m[2][index] = column.z;
    m[3][index] = column.w;
}

void Matrix4x4::SetSRT(const Vector3& scale, const Quaternion& rotation, const Vector3& translation)
{
#if 1
    *this = Matrix4x4::Rotate(rotation);
    _11 *= scale.x; _12 *= scale.x; _13 *= scale.x;
    _21 *= scale.y; _22 *= scale.y; _23 *= scale.y;
    _31 *= scale.z; _32 *= scale.z; _33 *= scale.z;
    _41 = translation.x;
    _42 = translation.y;
    _43 = translation.z;

#else
    *this = Scale(scale) * Rotate(rotation) * Translate(translation);
#endif
}

Matrix4x4 Matrix4x4::Inverse() const
{
    return XMMatrixInverse(nullptr, ToXMMATRIX());
}

Vector3 Matrix4x4::MultiplyPoint(const Vector3& point) const
{
    const Vector4 result
    (
        point.x * _11 + point.y * _21 + point.z * _31 + _41,
        point.x * _12 + point.y * _22 + point.z * _32 + _42,
        point.x * _13 + point.y * _23 + point.z * _33 + _43,
        point.x * _14 + point.y * _24 + point.z * _34 + _44
    );
    const float rhw = 1.0f / result.w;
    return Vector3(result.x * rhw, result.y * rhw, result.z * rhw);
}


Vector3 Matrix4x4::MultiplyPoint3x4(const Vector3& point) const
{
    return Vector3
    (
        point.x * _11 + point.y * _21 + point.z * _31 + _41,
        point.x * _12 + point.y * _22 + point.z * _32 + _42,
        point.x * _13 + point.y * _23 + point.z * _33 + _43
    );
}


Vector3 Matrix4x4::MultiplyVector(const Vector3& vector) const
{
    return Vector3
    (
        vector.x * _11 + vector.y * _21 + vector.z * _31,
        vector.x * _12 + vector.y * _22 + vector.z * _32,
        vector.x * _13 + vector.y * _23 + vector.z * _33
    );
}

Matrix4x4 Matrix4x4::Scale(float sx, float sy, float sz)
{
    return Matrix4x4
    (
        sx,  0,  0, 0,
         0, sy,  0, 0,
         0,  0, sz, 0,
         0,  0,  0, 1
    );
}

Matrix4x4 Matrix4x4::Scale(const Vector3& scale)
{
    return Scale(scale.x, scale.y, scale.z);
}

Matrix4x4 Matrix4x4::Rotate(const Quaternion& q)
{
    return XMMatrixRotationQuaternion(q.ToXMVECTOR());
}

Matrix4x4 Matrix4x4::Translate(float x, float y, float z)
{
    return Matrix4x4
    (
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        x, y, z, 1
    );
}

Matrix4x4 Matrix4x4::Translate(const Vector3& translation)
{
    return Translate(translation.x, translation.y, translation.z);
}

Matrix4x4 Matrix4x4::Ortho(float l, float r, float b, float t, float n, float f)
{
    return Matrix4x4
    (
        2.0f / (r - l), 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f / (t - b), 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f / (f - n), 0.0f,
        (l + r) / (l - r), (t + b) / (b - t), n / (n - f), 1.0f
    );
}

Matrix4x4 Matrix4x4::Perspective(float fov, float aspect, float zNear, float zFar)
{
    const float sy = 1.0f / Mathf::Tan(fov * Mathf::Deg2Rad * 0.5f);
    const float sx = sy / aspect;
    const float n = zNear;
    const float f = zFar;

    return Matrix4x4
    (
        sx, 0.0f, 0.0f, 0.0f,
        0.0f, sy, 0.0f, 0.0f,
        0.0f, 0.0f, f / (f - n), 1.0f,
        0.0f, 0.0f, -(f * n) / (f - n), 0.0f
    );
}

Matrix4x4 operator*(const Matrix4x4& lhs, const Matrix4x4& rhs)
{
#if 0
    // 2つの行列 lhs と rhs を乗算して新しい行列 result を作成する。
    Matrix4x4 result;

    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            result.m[y][x] = lhs.m[y][0] * rhs.m[0][x] +
                lhs.m[y][1] * rhs.m[1][x] +
                lhs.m[y][2] * rhs.m[2][x] +
                lhs.m[y][3] * rhs.m[3][x];
        }
    }

    return result;
#else
    return Matrix4x4
    (
        lhs._11 * rhs._11 + lhs._12 * rhs._21 + lhs._13 * rhs._31 + lhs._14 * rhs._41,
        lhs._11 * rhs._12 + lhs._12 * rhs._22 + lhs._13 * rhs._32 + lhs._14 * rhs._42,
        lhs._11 * rhs._13 + lhs._12 * rhs._23 + lhs._13 * rhs._33 + lhs._14 * rhs._43,
        lhs._11 * rhs._14 + lhs._12 * rhs._24 + lhs._13 * rhs._34 + lhs._14 * rhs._44,

        lhs._21 * rhs._11 + lhs._22 * rhs._21 + lhs._23 * rhs._31 + lhs._24 * rhs._41,
        lhs._21 * rhs._12 + lhs._22 * rhs._22 + lhs._23 * rhs._32 + lhs._24 * rhs._42,
        lhs._21 * rhs._13 + lhs._22 * rhs._23 + lhs._23 * rhs._33 + lhs._24 * rhs._43,
        lhs._21 * rhs._14 + lhs._22 * rhs._24 + lhs._23 * rhs._34 + lhs._24 * rhs._44,

        lhs._31 * rhs._11 + lhs._32 * rhs._21 + lhs._33 * rhs._31 + lhs._34 * rhs._41,
        lhs._31 * rhs._12 + lhs._32 * rhs._22 + lhs._33 * rhs._32 + lhs._34 * rhs._42,
        lhs._31 * rhs._13 + lhs._32 * rhs._23 + lhs._33 * rhs._33 + lhs._34 * rhs._43,
        lhs._31 * rhs._14 + lhs._32 * rhs._24 + lhs._33 * rhs._34 + lhs._34 * rhs._44,

        lhs._41 * rhs._11 + lhs._42 * rhs._21 + lhs._43 * rhs._31 + lhs._44 * rhs._41,
        lhs._41 * rhs._12 + lhs._42 * rhs._22 + lhs._43 * rhs._32 + lhs._44 * rhs._42,
        lhs._41 * rhs._13 + lhs._42 * rhs._23 + lhs._43 * rhs._33 + lhs._44 * rhs._43,
        lhs._41 * rhs._14 + lhs._42 * rhs._24 + lhs._43 * rhs._34 + lhs._44 * rhs._44
    );
#endif
}
