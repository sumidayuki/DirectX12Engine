#pragma once

/// <summary>
/// ���w���Ǘ�����N���X�ł��B
/// </summary>
class Mathf
{
public:
	// �~���� ��
	static constexpr float PI = 3.1415926f;

	// �x�����W�A���ɕϊ����邽�߂̌W��
	static constexpr float Deg2Rad = Mathf::PI / 180;

	// ���W�A����x�ɕϊ����邽�߂̌W��
	static constexpr float Rad2Deg = 180 / Mathf::PI;

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
};