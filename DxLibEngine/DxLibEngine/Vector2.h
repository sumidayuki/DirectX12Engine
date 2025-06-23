#pragma once

class Vector2
{
public:
	// �S�Ă̐����� 0 �ɐݒ肳�ꂽ�񎟌��x�N�g��(0, 0)�ł��B
	static const Vector2 zero;

	// �S�Ă̐����� 1 �ɐݒ肳�ꂽ�񎟌��x�N�g��(1, 1)�ł��B
	static const Vector2 one;

	static const Vector2 right;

	static const Vector2 left;

	static const Vector2 up;

	static const Vector2 down;

public:
	union
	{
		// ����
		struct
		{
			float x;
			float y;
		};

		// �z��\��
		int components[2];
	};


public:
	// �f�t�H���g�R���X�g���N�^
	Vector2() = default;

	// �V�����񎟌��x�N�g�����쐬���܂��B
	Vector2(float x, float y);

	float Magnitude() const;

	Vector2 Normalized() const;

	Vector2& operator +() const;

	Vector2 operator -() const;

	Vector2& operator +=(const Vector2& rhs);

	Vector2& operator -=(const Vector2& rhs);

	Vector2& operator *=(const float scalar);

	Vector2& operator *=(const Vector2& rhs);

	Vector2& operator /=(const float divisor);

	Vector2& operator /=(const Vector2& rhs);
};

Vector2 operator +(const Vector2& lhs, const Vector2& rhs);

Vector2 operator -(const Vector2& lhs, const Vector2& rhs);

Vector2 operator *(const Vector2& vector, float scalar);

Vector2 operator *(float scalar, const Vector2& vector);

Vector2 operator /(const Vector2& vector, float divisor);