#pragma once

// ���Ƃō��܂����B
// Swap�֐���float�^�ł����ł��Ȃ��������̂��e���v���[�g�����܂����B

/// <summary>
/// ���w���Ǘ�����N���X�ł��B
/// </summary>
class Mathf
{
public:
	// �~������ (float�^�̗L��������7��)
	static constexpr float PI = 3.1415926f;

	// �΂�2�{�̒l��\���܂��B
	static constexpr float TwoPI = 6.283185307f;

	// �΂�2�Ŋ������l(��/2)��\���܂��B
	static constexpr float PIOver2 = 1.570796327f;

	// �΂�4�Ŋ������l(��/4)��\���܂��B
	static constexpr float PIOver4 = 0.785398163f;

	// ���R�ΐ��̒�e(�l�C�s�A��)��\���܂��B
	static constexpr float E = 2.7182818f;

	// 2���Ƃ���e�̑ΐ���\���܂��B
	static constexpr float Log2E = 1.4426950408889634f;

	// 10���Ƃ���e�̑ΐ���\���܂��B
	static constexpr float Log10E = 0.4342944819032518f;

	// �x�����W�A���ɕϊ����邽�߂̌W��
	static constexpr float Deg2Rad = Mathf::PI / 180.0f;

	// ���W�A����x�ɕϊ����邽�߂̌W��
	static constexpr float Rad2Deg = 180.0f / Mathf::PI;

	// �v�Z�@�C�v�V���� (�ɂ߂� 0 �ɋ߂������Ȓl)
	static constexpr float Epsilon = std::numeric_limits<float>::epsilon();

	// ���̖������\���܂��B
	static constexpr float Infinity = std::numeric_limits<float>::infinity();

	// ���̖������\���܂��B
	static constexpr float NegativeInfinity = -Infinity;

public:
	/// <summary>
	/// ��Βl��Ԃ��܂��B
	/// </summary>
	/// <param name="value"></param>
	/// <returns></returns>
	static constexpr float Abs(float value) { return (value < 0.0f) ? -value : value; }

	/// <summary>
	/// �����̒l��Ԃ��܂��B(�p�x�̓��W�A��)
	/// </summary>
	/// <param name="radians"></param>
	/// <returns></returns>
	static float Sin(float radians) { return sinf(radians); }

	/// <summary>
	/// �]���̒l��Ԃ��܂��B(�p�x�̓��W�A��)
	/// </summary>
	/// <param name="radians"></param>
	/// <returns></returns>
	static float Cos(float radians) { return cosf(radians); }

	/// <summary>
	/// ���ڂ̒l��Ԃ��܂��B(�p�x�̓��W�A��)
	/// </summary>
	/// <param name="radians"></param>
	/// <returns></returns>
	static float Tan(float radians) { return tanf(radians); }

	/// <summary>
	/// �t���ڂ̒l��Ԃ��܂��B
	/// </summary>
	/// <param name="y"></param>
	/// <param name="x"></param>
	/// <returns></returns>
	static float Atan2(float y, float x) { return atan2f(y, x); }

	/// <summary>
	/// �������̒l��Ԃ��܂��B
	/// </summary>
	/// <param name="value"></param>
	/// <returns></returns>
	static float Sqrt(float value) { return sqrt(value); }

	/// <summary>
	/// 2�̒l���������܂��B
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="a"></param>
	/// <param name="b"></param>
	template<typename T>
	static constexpr void Swap(T& a, T& b) { const T temp = a; a = b; b = temp; }

	// value��[minValue, maxValue]�͈̔͂ɐ��������l��Ԃ��܂��B
	static constexpr float Clamp(float value, float minValue, float maxValue) { return (value < minValue) ? minValue : ((value > maxValue) ? maxValue : value); }

	// [a, b]�̊ԂŃp�����[�^�[ t �ɂ����`��Ԃ��܂��Bt ��[0, 1]�ɐ�������܂��B
	//�uLerp�v�͐��`���(Linear Interpolate)�̗��ł��B
	static constexpr float Lerp(float a, float b, float t) { return LerpUnclamped(a, b, Clamp(t, 0, 1)); }


	/// <summary>
	/// [a, b]�̊ԂŃp�����[�^�[ t �ɂ����`��Ԃ��܂��Bt �ɐ����͂���܂���B
	/// </summary>
	/// <param name="a"></param>
	/// <param name="b"></param>
	/// <param name="t"></param>
	/// <returns></returns>
	static constexpr float LerpUnclamped(float a, float b, float t) { return a + (b - a) * t; }
};