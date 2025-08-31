#pragma once

// ���Ƃō��܂����B

class Vector3Int
{
public:
	// �S�Ă̐����� 0 �ɐݒ肳�ꂽ�O�����x�N�g���i0, 0, 0�j�ł��B
	static const Vector3Int zero;

	// �S�Ă̐����� 1 �ɐݒ肳�ꂽ�O�����x�N�g���i1, 1, 1�j�ł��B
	static const Vector3Int one;

	static const Vector3Int right;

	static const Vector3Int left;

	static const Vector3Int up;

	static const Vector3Int down;

public:
	union
	{
		// ����
		struct
		{
			int x;
			int y;
			int z;
		};

		int components[3];
	};

public:
	// �f�t�H���g�R���X�g���N�^
	Vector3Int() = default;

	// �����t���R���X�g���N�^
	Vector3Int(int x, int y, int z);

	int Magnitude() const;

	Vector3Int& operator +() const;

	Vector3Int operator -() const;

	Vector3Int& operator +=(const Vector3Int& rhs);

	Vector3Int& operator -=(const Vector3Int& rhs);

	Vector3Int& operator *=(const int scalar);

	Vector3Int& operator *=(const Vector3Int& rhs);

	Vector3Int& operator /=(const int divisor);

	Vector3Int& operator /=(const Vector3Int& rhs);

};

Vector3Int operator +(const Vector3Int& lhs, const Vector3Int& rhs);

Vector3Int operator -(const Vector3Int& lhs, const Vector3Int& rhs);

Vector3Int operator *(const Vector3Int& vector, int scalar);

Vector3Int operator *(int scalar, const Vector3Int& vector);

Vector3Int operator /(const Vector3Int& vector, int divisor);