#pragma once

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
};

[[nodiscard]] Quaternion operator *(const Quaternion& lhs, const Quaternion& rhs) noexcept;