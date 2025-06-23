#pragma once

class Vector2Int
{
public:
	// 全ての成分が 0 に設定された二次元ベクトル(0, 0)です。
	static const Vector2Int zero;

	// 全ての成分が 1 に設定された二次元ベクトル(1, 1)です。
	static const Vector2Int one;

	static const Vector2Int right;

	static const Vector2Int left;

	static const Vector2Int up;

	static const Vector2Int down;

public:
	union
	{
		// 成分
		struct
		{
			int x;
			int y;
		};

		// 配列表現
		int components[2];
	};


public:
	// デフォルトコンストラクタ
	Vector2Int() = default;

	// 新しい二次元ベクトルを作成します。
	Vector2Int(int x, int y);

	Vector2Int& operator +() const;

	Vector2Int operator -() const;
};

Vector2Int operator +(const Vector2Int& lhs, const Vector2Int& rhs);

Vector2Int operator -(const Vector2Int& lhs, const Vector2Int& rhs);

Vector2Int operator *(const Vector2Int& vector, int scalar);

Vector2Int operator *(int scalar, const Vector2Int& vector);

Vector2Int operator /(const Vector2Int& vector, int divisor);