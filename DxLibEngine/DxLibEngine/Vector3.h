#pragma once

class Vector3 
{
public:
	// �S�Ă̐����� 0 �ɐݒ肳�ꂽ�O�����x�N�g���i0, 0, 0�j�ł��B
	static const Vector3 zero;

	// �S�Ă̐����� 1 �ɐݒ肳�ꂽ�O�����x�N�g���i1, 1, 1�j�ł��B
	static const Vector3 one;

	static const Vector3 right;

	static const Vector3 left;

	static const Vector3 up;

	static const Vector3 down;

	static const Vector3 forward;

public:
	union
	{
		struct 
		{
			float x;
			float y;
			float z;
		};

		int components[3];
	};

public:
	Vector3() = default;

	Vector3(float x, float y, float z);

	float Magnitude() const;

	Vector3 Normalized() const;

	static Vector3 Scale(const Vector3& a, const Vector3& b);

	Vector3& operator +() const;

	Vector3 operator -() const;

	Vector3& operator +=(const Vector3& rhs);

	Vector3& operator -=(const Vector3& rhs);

	Vector3& operator *=(const float scalar);

	Vector3& operator *=(const Vector3& rhs);

	Vector3& operator /=(const float divisor);

	Vector3& operator /=(const Vector3& rhs);
};

Vector3 operator +(const Vector3& lhs, const Vector3& rhs);

Vector3 operator -(const Vector3& lhs, const Vector3& rhs);

Vector3 operator *(const Vector3& vector, float scalar);

Vector3 operator *(float scalar, const Vector3& vector);

Vector3 operator /(const Vector3& vector, float divisor);