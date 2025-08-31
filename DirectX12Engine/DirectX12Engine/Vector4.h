#pragma once

// éˆã∆Ç≈çÏÇËÇ‹ÇµÇΩÅB

class Vector4
{
public:
	static const Vector4 zero;

public:
	union
	{
		struct
		{
			float x;
			float y;
			float z;
			float w;
		};

		float components[4];
	};

public:
	Vector4() = default;

	Vector4(float x, float y);

	Vector4(float x, float y, float z);

	Vector4(float x, float y, float z, float w);

	Vector4(const int components[4]);

	Vector4(const float components[4]);

	Vector4(const Vector2& vector, float z, float w);

	Vector4(const Vector3& vector, float w);
};