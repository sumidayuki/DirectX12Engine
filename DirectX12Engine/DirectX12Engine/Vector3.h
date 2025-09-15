#pragma once

// ���Ƃō��܂����B

/// <summary>
/// �O�����x�N�g����\���N���X�ł��B
/// </summary>
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

	Vector3(const Vector2& vector2, float z);

	// XMVECTOR�^�̒l�ŏ��������ꂽ�V����3�����x�N�g�����쐬���܂��B
	Vector3(const DirectX::XMVECTOR& xmvector);

	// XMFLOAT3�^�̒l�ŏ��������ꂽ�V����3�����x�N�g�����쐬���܂��B
	Vector3(const DirectX::XMFLOAT3& xmvector);

	// ����3�����x�N�g���̐���(x, y, z)��ݒ肵�܂��B
	void Set(float newX, float newY, float newZ);

	// Vector3�^����XMVECTOR�^�ɕϊ����܂��B
	DirectX::XMVECTOR ToXMVECTOR() const;

	float Magnitude() const;

	// ����3�����x�N�g���̑傫��(�m����)��2���Ԃ��܂��B
	float SqrMagnitude() const;

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

	// 3�����x�N�g���̑傫�����w�肳�ꂽ�͈͂ɃN�����v�����V����3�����x�N�g�����쐬���܂��B
	static Vector3 ClampMagnitude(const Vector3& vector, float minValue, float maxValue);

	// 2�̃x�N�g���̊O�ς�Ԃ��܂��B
	static Vector3 Cross(const Vector3& lhs, const Vector3& rhs);

	// 2��3�����x�N�g���Ԃ���`��Ԃ��A�V����3�����x�N�g�����쐬���܂��B
	static Vector3 Lerp(const Vector3& a, const Vector3& b, float t);

	// 2��3�����x�N�g���Ԃ���`��Ԃ��A�V����3�����x�N�g�����쐬���܂��B
	static Vector3 LerpUnclamped(const Vector3& a, const Vector3& b, float t);

	// ���K�����ꂽ�V����3�����x�N�g�����쐬���܂��B
	static Vector3 Normalize(const Vector3& vector);
};

Vector3 operator +(const Vector3& lhs, const Vector3& rhs);

Vector3 operator -(const Vector3& lhs, const Vector3& rhs);

Vector3 operator *(const Vector3& lhs, const Vector3& rhs);

Vector3 operator *(const Vector3& vector, float scalar);

Vector3 operator *(float scalar, const Vector3& vector);

Vector3 operator /(const Vector3& vector, float divisor);