#include "Vector2Int.h"

// ê√ìIÉÅÉìÉoïœêîÇÃé¿ë‘ÇêÈåæ
const Vector2Int Vector2Int::zero(0, 0);
const Vector2Int Vector2Int::one(1, 1);
const Vector2Int Vector2Int::right(1, 0);
const Vector2Int Vector2Int::left(-1, 0);
const Vector2Int Vector2Int::up(0, -1);
const Vector2Int Vector2Int::down(0, 1);

Vector2Int::Vector2Int(int x, int y)
	: x(x)
	, y(y)
{
}

Vector2Int& Vector2Int::operator+() const
{
	return const_cast<Vector2Int&>(*this);
}

Vector2Int Vector2Int::operator-() const
{
	return Vector2Int(-x, -y);
}

Vector2Int operator+(const Vector2Int& lhs, const Vector2Int& rhs)
{
	return Vector2Int(lhs.x + rhs.x, lhs.y + rhs.y);
}

Vector2Int operator-(const Vector2Int& lhs, const Vector2Int& rhs)
{
	return Vector2Int(lhs.x - rhs.x, lhs.y - lhs.y);
}

Vector2Int operator*(const Vector2Int& vector, int scalar)
{
	return Vector2Int(vector.x * scalar, vector.y * scalar);
}

Vector2Int operator*(int scalar, const Vector2Int& vector)
{
	return Vector2Int(scalar * vector.x, scalar * vector.y);
}

Vector2Int operator/(const Vector2Int& vector, int divisor)
{
	return Vector2Int(vector.x / divisor, vector.y / divisor);
}