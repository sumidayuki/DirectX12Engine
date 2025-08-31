#pragma once

// 授業で作りました。

class Vector3Int
{
public:
	// 全ての成分が 0 に設定された三次元ベクトル（0, 0, 0）です。
	static const Vector3Int zero;

	// 全ての成分が 1 に設定された三次元ベクトル（1, 1, 1）です。
	static const Vector3Int one;

	static const Vector3Int right;

	static const Vector3Int left;

	static const Vector3Int up;

	static const Vector3Int down;

public:
	union
	{
		// 成分
		struct
		{
			int x;
			int y;
			int z;
		};

		int components[3];
	};

public:
	// デフォルトコンストラクタ
	Vector3Int() = default;

	// 引数付きコンストラクタ
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