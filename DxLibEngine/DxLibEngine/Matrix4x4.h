#pragma once

// ���Ƃō��܂����B

class Matrix4x4
{
public:
	static const Matrix4x4 zero;

	static const Matrix4x4 identity;

public:
	union
	{
		struct
		{
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
		};

		Vector4 rows[4];

		float m[4][4];
	};

public:
	Matrix4x4() = default;

	Matrix4x4(const float m[4][4]);

	Matrix4x4
	(
		float _11, float _12, float _13, float _14,
		float _21, float _22, float _23, float _24,
		float _31, float _32, float _33, float _34,
		float _41, float _42, float _43, float _44
	);

	Matrix4x4(const DirectX::XMMATRIX& xmmatrix);

	DirectX::XMMATRIX ToXMMATRIX() const;

	// ���̍s��̍s��ݒ肵�܂��B
	void SetRow(int index, const Vector4& row);

	// ���̍s��̗��ݒ肵�܂��B
	void SetColumn(int index, const Vector4& column);

	void SetSRT(const Vector3& scale, const Quaternion& rotation, const Vector3& translation);

	// ���̍s��̓]�u�s���Ԃ��܂��B
	Matrix4x4 Transpose() const;

	// ���̍s��ŋt�s���Ԃ��܂��B
	Matrix4x4 Inverse() const;

	// ���̍s��ňʒu�x�N�g����ϊ����܂��B
	Vector3 MultiplyPoint(const Vector3& point) const;

	// ���̍s��ňʒu�x�N�g����ϊ����܂��B (�ˉe�ϊ��s�񓙂ł͎g�p�ł��܂���)
	Vector3 MultiplyPoint3x4(const Vector3& point) const;

	// ���̍s��ŕ����x�N�g����ϊ����܂��B
	Vector3 MultiplyVector(const Vector3& vector) const;

	static Matrix4x4 Scale(float x, float y, float z);
	
	static Matrix4x4 Scale(const Vector3& scale);

	static Matrix4x4 Rotate(const Quaternion& q);

	static Matrix4x4 Translate(float x, float y, float z);

	static Matrix4x4 Translate(const Vector3& translation);

	// ���s���e��\���v���W�F�N�V�����ϊ��s����쐬���܂��B
	static Matrix4x4 Ortho(float left, float right, float bottom, float top, float zNear, float zFar);

	// �������e��\���v���W�F�N�V�����ϊ��s����쐬���܂��B
	static Matrix4x4 Perspective(float fov, float aspect, float zNear, float zFar);
};

Matrix4x4 operator *(const Matrix4x4& lhs, const Matrix4x4& rhs);