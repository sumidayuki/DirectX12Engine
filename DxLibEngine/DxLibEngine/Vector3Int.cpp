#include "Vector3Int.h"

// ê√ìIÉÅÉìÉoïœêîÇÃé¿ëÃÇêÈåæ
const Vector3Int Vector3Int::zero(0, 0, 0);
const Vector3Int Vector3Int::one(1, 1, 1);
const Vector3Int Vector3Int::right(1, 0, 0);
const Vector3Int Vector3Int::left(-1, 0, 0);
const Vector3Int Vector3Int::up(-1, 0, 0);
const Vector3Int Vector3Int::down(1, 0, 0);

Vector3Int::Vector3Int(int x, int y, int z)
    : x(x)
    , y(y)
    , z(z)
{
}

int Vector3Int::Magnitude() const
{
    return Mathf::Sqrt((float)(x * x + y * y + z * z));
}

Vector3Int& Vector3Int::operator+() const
{
    return const_cast<Vector3Int&>(*this);
}

Vector3Int Vector3Int::operator-() const
{
    return Vector3Int(-x, -y, -z);
}

Vector3Int& Vector3Int::operator+=(const Vector3Int& rhs)
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
}

Vector3Int& Vector3Int::operator-=(const Vector3Int& rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
}

Vector3Int& Vector3Int::operator*=(const int scalar)
{
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

Vector3Int& Vector3Int::operator*=(const Vector3Int& rhs)
{
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
    return *this;
}

Vector3Int& Vector3Int::operator/=(const int divisor)
{
    x /= divisor;
    y /= divisor;
    z /= divisor;
    return *this;
}

Vector3Int& Vector3Int::operator/=(const Vector3Int& rhs)
{
    x /= rhs.x;
    y /= rhs.y;
    z /= rhs.z;
    return *this;
}

Vector3Int operator+(const Vector3Int& lhs, const Vector3Int& rhs)
{
    return Vector3Int(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

Vector3Int operator-(const Vector3Int& lhs, const Vector3Int& rhs)
{
    return Vector3Int(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

Vector3Int operator*(const Vector3Int& vector, int scalar)
{
    return Vector3Int(vector.x * scalar, vector.y * scalar, vector.z * scalar);
}

Vector3Int operator*(int scalar, const Vector3Int& vector)
{
    return Vector3Int(scalar * vector.x, scalar * vector.y, scalar * vector.z);
}

Vector3Int operator/(const Vector3Int& vector, int divisor)
{
    return Vector3Int(vector.x / divisor, vector.y / divisor, vector.z / divisor);
}
