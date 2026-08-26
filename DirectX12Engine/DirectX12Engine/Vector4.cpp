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

Vector4::Vector4(const Color& color)
	: x(color.r)
	, y(color.g)
	, z(color.b)
	, w(color.a)
{
}

Vector4 Vector4::Lerp(const Vector4& a, const Vector4& b, float t)
{
    return Vector4
	(
		Mathf::Lerp(a.x, b.x, t),
		Mathf::Lerp(a.y, b.y, t),
		Mathf::Lerp(a.z, b.z, t),
		Mathf::Lerp(a.w, b.w, t)
	);
}
