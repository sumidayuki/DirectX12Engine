#include "Vector2.h"

// ê√ìIÉÅÉìÉoïœêîÇÃé¿ëÃÇêÈåæ
const Vector2 Vector2::zero(0.0f, 0.0f);
const Vector2 Vector2::one(1.0f, 1.0f);
const Vector2 Vector2::right(1.0f, 0.0f);
const Vector2 Vector2::left(-1.0f, 0.0f);
const Vector2 Vector2::up(0.0f, 1.0f);
const Vector2 Vector2::down(0.0f, -1.0f);
const Vector2 Vector2::negativeInfinity(Mathf::NegativeInfinity, Mathf::NegativeInfinity);
const Vector2 Vector2::positiveInfinity(Mathf::Infinity, Mathf::Infinity);

Vector2::Vector2(float x, float y)
    : x(x)
    , y(y)
{
}

float Vector2::Magnitude() const
{
    return Mathf::Sqrt(x * x + y * y);
}

Vector2 Vector2::Normalized() const
{
    const float reciprocal = 1.0f / Magnitude();
    return Vector2(x * reciprocal, y * reciprocal);
}

Vector2& Vector2::operator+() const
{
    return const_cast<Vector2&>(*this);
}

Vector2 Vector2::operator-() const
{
    return Vector2(-x, -y);
}

Vector2& Vector2::operator+=(const Vector2& rhs)
{
    x += rhs.x;
    y += rhs.y;
    return *this;
}

Vector2& Vector2::operator-=(const Vector2& rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

Vector2& Vector2::operator*=(const float scalar)
{
    x *= scalar;
    y *= scalar;
    return *this;
}

Vector2& Vector2::operator*=(const Vector2& rhs)
{
    x *= rhs.x;
    y *= rhs.y;
    return *this;
}

Vector2& Vector2::operator/=(const float divisor)
{
    x /= divisor;
    y /= divisor;
    return *this;
}

Vector2& Vector2::operator/=(const Vector2& rhs)
{
    x /= rhs.x;
    y /= rhs.y;
    return *this;
}

Vector2 operator+(const Vector2& lhs, const Vector2& rhs)
{
    return Vector2(lhs.x + rhs.x, lhs.y + rhs.y);
}

Vector2 operator-(const Vector2& lhs, const Vector2& rhs)
{
    return Vector2(lhs.x - rhs.x, lhs.y - rhs.y);
}

Vector2 operator*(const Vector2& vector, float scalar)
{
    return Vector2(vector.x * scalar, vector.y * scalar);
}

Vector2 operator*(float scalar, const Vector2& vector)
{
    return Vector2(scalar * vector.x, scalar * vector.y);
}

Vector2 operator/(const Vector2& vector, float divisor)
{
    return Vector2(vector.x / divisor, vector.y / divisor);
}