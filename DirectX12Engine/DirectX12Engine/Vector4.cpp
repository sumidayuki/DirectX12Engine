#include "Vector4.h"

// Ã“Iƒƒ“ƒo•Ï”‚ÌÀ‘Ì‚ğéŒ¾
const Vector4 Vector4::zero(0.0f, 0.0f, 0.0f, 0.0f);

Vector4::Vector4(float x, float y)
	: x(x)
	, y(y)
	, z(0.0f)
	, w(0.0f)
{
}

Vector4::Vector4(float x, float y, float z)
	: x(x)
	, y(y)
	, z(z)
	, w(0.0f)
{
}

Vector4::Vector4(float x, float y, float z, float w)
	: x(x)
	, y(y)
	, z(z)
	, w(w)
{
}

Vector4::Vector4(const Vector2& vector, float z, float w)
	: x(vector.x)
	, y(vector.y)
	, z(z)
	, w(w)
{
}

Vector4::Vector4(const Vector3& vector, float w)
	: x(vector.x)
	, y(vector.y)
	, z(vector.z)
	, w(w)
{
}
