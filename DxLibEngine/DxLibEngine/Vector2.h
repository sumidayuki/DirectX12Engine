#pragma once

class Vector2
{
public:
	// 全ての成分が 0 に設定された二次元ベクトル(0, 0)です。
	static const Vector2 zero;

	// 全ての成分が 1 に設定された二次元ベクトル(1, 1)です。
	static const Vector2 one;

	static const Vector2 right;

	static const Vector2 left;

	static const Vector2 up;

	static const Vector2 down;

public:
	union
	{
		// 成分
		struct
		{
			float x;
			float y;
		};

		// 配列表現
		int components[2];
	};


public:
	// デフォルトコンストラクタ
	Vector2() = default;

	// 新しい二次元ベクトルを作成します。
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