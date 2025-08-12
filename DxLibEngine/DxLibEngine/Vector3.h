#pragma once

// 授業で作りました。

/// <summary>
/// 三次元ベクトルを表すクラスです。
/// </summary>
class Vector3 
{
public:
	// 全ての成分が 0 に設定された三次元ベクトル（0, 0, 0）です。
	static const Vector3 zero;

	// 全ての成分が 1 に設定された三次元ベクトル（1, 1, 1）です。
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

	Vector3(const Vector2& vector2, float z);

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

	// 2つのベクトルの外積を返します。
	static Vector3 Cross(const Vector3& lhs, const Vector3& rhs);

	// 正規化された新しい3次元ベクトルを作成します。
	static Vector3 Normalize(const Vector3& vector);
};

Vector3 operator +(const Vector3& lhs, const Vector3& rhs);

Vector3 operator -(const Vector3& lhs, const Vector3& rhs);

Vector3 operator *(const Vector3& vector, float scalar);

Vector3 operator *(float scalar, const Vector3& vector);

Vector3 operator /(const Vector3& vector, float divisor);