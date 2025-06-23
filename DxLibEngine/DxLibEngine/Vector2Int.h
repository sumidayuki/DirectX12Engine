#pragma once

class Vector2Int
{
public:
	// �S�Ă̐����� 0 �ɐݒ肳�ꂽ�񎟌��x�N�g��(0, 0)�ł��B
	static const Vector2Int zero;

	// �S�Ă̐����� 1 �ɐݒ肳�ꂽ�񎟌��x�N�g��(1, 1)�ł��B
	static const Vector2Int one;

	static const Vector2Int right;

	static const Vector2Int left;

	static const Vector2Int up;

	static const Vector2Int down;

public:
	union
	{
		// ����
		struct
		{
			int x;
			int y;
		};

		// �z��\��
		int components[2];
	};


public:
	// �f�t�H���g�R���X�g���N�^
	Vector2Int() = default;

	// �V�����񎟌��x�N�g�����쐬���܂��B
	Vector2Int(int x, int y);

	Vector2Int& operator +() const;

	Vector2Int operator -() const;
};

Vector2Int operator +(const Vector2Int& lhs, const Vector2Int& rhs);

Vector2Int operator -(const Vector2Int& lhs, const Vector2Int& rhs);

Vector2Int operator *(const Vector2Int& vector, int scalar);

Vector2Int operator *(int scalar, const Vector2Int& vector);

Vector2Int operator /(const Vector2Int& vector, int divisor);