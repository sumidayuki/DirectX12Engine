#pragma once

// ���Ƃō��܂����B

/// <summary>
/// �l������\���N���X�ł��B
/// </summary>
class Quaternion
{
public:
	// �P���l���� (��]�Ȃ�)
	static const Quaternion identity;

public:
	union
	{
		// �����\��
		struct
		{
			float x;
			float y;
			float z;
			float w;
		};

		// �i�����x�N�g���A�����j�\��
		struct
		{
			Vector3 imaginary;
			float real;
		};

		// �z��\��
		int components[4];
	};

public:
	// �f�t�H���g�R���X�g���N�^
	Quaternion() = default;

	/// <summary>
	/// �V�����N�H�[�^�j�I��(x, y, z, w)���쐬���܂��B
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <param name="z"></param>
	/// <param name="w"></param>
	Quaternion(float x, float y, float z, float w);

	// XMVECTOR�^�̒l�ŏ��������ꂽ�V�����N�H�[�^�j�I�����쐬���܂��B
	Quaternion(const DirectX::XMVECTOR& xmquat);

	/// <summary>
	/// Quaternion�^����XMVECTOR�^�ɕϊ����܂��B
	/// </summary>
	/// <returns></returns>
	[[nodiscard]] DirectX::XMVECTOR ToXMVECTOR() const;

	/// <summary>
	/// ��]�� axis �̎���� angle �x������]������N�H�[�^�j�I�����쐬���܂��B (�P�ʂ̓f�B�O���[)
	/// </summary>
	/// <param name="angle"></param>
	/// <param name="axis"></param>
	/// <returns></returns>
	[[nodiscard]] static Quaternion AngleAxis(float angle, const Vector3& axis);

	// �I�C���[�p�\���ŃN�H�[�^�j�I�����쐬���܂��B (�P�ʂ̓f�B�O���[)
	//	Unity�̏ꍇ : Z-Y-X�̏��ŉ�]
	//	DirectX�̏ꍇ : Z-X-Y�̏��ŉ�]
	[[nodiscard]] static Quaternion Euler(float x, float y, float z);

	// �I�C���[�p�\���ŃN�H�[�^�j�I�����쐬���܂��B (�P�ʂ̓f�B�O���[)
	//  Unity�̏ꍇ : Z-Y-X�̏��ŉ�]
	//	DirectX�̏ꍇ : Z-X-Y�̏��ŉ�]
	[[nodiscard]] static Quaternion Euler(const Vector3& euler);

	// �w�肳�ꂽ forward �� upwards �����ɉ�]���܂��B
	[[nodiscard]] static Quaternion LookRotation(const Vector3& forward, const Vector3& upwards = Vector3::up);

	// �l���� a �� b �̊Ԃ��p�����[�^�[ t ���g���ċ��ʐ��`��Ԃ�����A���̌��ʂ𐳋K�������l������Ԃ��܂��B
	// �p�����[�^�[ t ��[0,1]�͈̔͂ɃN�����v����܂��B
	[[nodiscard]] static Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t);

};

[[nodiscard]] Quaternion operator *(const Quaternion& lhs, const Quaternion& rhs) noexcept;

[[nodescard]] Vector3 operator *(const Quaternion& q, const Vector3& v);