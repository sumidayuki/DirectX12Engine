#include "Vector3.h"

// ê√ìIÉÅÉìÉoïœêîÇÃé¿ëÃÇêÈåæ
const Vector3 Vector3::zero(0, 0, 0);
const Vector3 Vector3::one(0, 0, 0);
const Vector3 Vector3::right(1, 0, 0);
const Vector3 Vector3::left(-1, 0, 0);
const Vector3 Vector3::up(0, 1, 0);
const Vector3 Vector3::down(0, -1, 0);
const Vector3 Vector3::forward(0, 0, 1);

Vector3::Vector3(float x, float y, float z)
    : x(x)
    , y(y)
    , z(z)
{
}

Vector3::Vector3(const Vector2& vector2, float z)
    : x(vector2.x)
    , y(vector2.y)
    , z(z)
{
}

float Vector3::Magnitude() const
{
    return Mathf::Sqrt(x * x + y * y + z * z);
}

Vector3 Vector3::Normalized() const
{
    const float reciprocal = 1.0f / Magnitude();
    return Vector3(x * reciprocal, y * reciprocal, z * reciprocal);
}

Vector3 Vector3::Scale(const Vector3& a, const Vector3& b)
{
    return Vector3(a.x * b.x, a.y * b.y, a.z * b.z);
}

Vector3& Vector3::operator+() const
{
    return const_cast<Vector3&>(*this);
}

Vector3 Vector3::operator-() const
{
    return Vector3(-x, -y, -z);
}

Vector3& Vector3::operator+=(const Vector3& rhs)
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
}

Vector3& Vector3::operator-=(const Vector3& rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
}

Vector3& Vector3::operator*=(const float scalar)
{
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

Vector3& Vector3::operator*=(const Vector3& rhs)
{
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
    return *this;
}

Vector3& Vector3::operator/=(const float divisor)
{
    x /= divisor;
    y /= divisor;
    z /= divisor;
    return *this;
}

Vector3& Vector3::operator/=(const Vector3& rhs)
{
    x /= rhs.x;
    y /= rhs.y;
    z /= rhs.z;
    return *this;
}

Vector3 Vector3::Cross(const Vector3& lhs, const Vector3& rhs)
{
    return Vector3
    (
        lhs.y * rhs.z - lhs.z * rhs.y,
        lhs.z * rhs.x - lhs.x * rhs.z,
        lhs.x * rhs.y - lhs.y * rhs.x
    );
}

Vector3 Vector3::Lerp(const Vector3& a, const Vector3& b, float t)
{
    return Vector3
    (
        Mathf::Lerp(a.x, b.x, t),
        Mathf::Lerp(a.y, b.y, t),
        Mathf::Lerp(a.z, b.z, t)
    );
}

Vector3 Vector3::LerpUnclamped(const Vector3& a, const Vector3& b, float t)
{
    return Vector3
    (
        Mathf::LerpUnclamped(a.x, b.x, t),
        Mathf::LerpUnclamped(a.y, b.y, t),
        Mathf::LerpUnclamped(a.z, b.z, t)
    );
}

Vector3 Vector3::Normalize(const Vector3& vector)
{
    return vector.Normalized();
}

Vector3 operator+(const Vector3& lhs, const Vector3& rhs)
{
    return Vector3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

Vector3 operator-(const Vector3& lhs, const Vector3& rhs)
{
    return Vector3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

Vector3 operator*(const Vector3& vector, float scalar)
{
    return Vector3(vector.x * scalar, vector.y * scalar, vector.x * scalar);
}

Vector3 operator*(float scalar, const Vector3& vector)
{
    return Vector3(scalar * vector.x, scalar * vector.y, scalar * vector.z);
}

Vector3 operator/(const Vector3& vector, float divisor)
{
    return Vector3(vector.x / divisor, vector.y / divisor, vector.z / divisor);
}