#pragma once

/// <summary>
/// 数学を管理するクラスです。
/// </summary>
class Mathf
{
public:
	// 円周率 π
	static constexpr float PI = 3.1415926f;

	// 度をラジアンに変換するための係数
	static constexpr float Deg2Rad = Mathf::PI / 180;

	// ラジアンを度に変換するための係数
	static constexpr float Rad2Deg = 180 / Mathf::PI;

public:
	/// <summary>
	/// 絶対値を返します。
	/// </summary>
	/// <param name="value"></param>
	/// <returns></returns>
	static constexpr float Abs(float value) { return (value < 0.0f) ? -value : value; }

	/// <summary>
	/// 正弦の値を返します。(角度はラジアン)
	/// </summary>
	/// <param name="radians"></param>
	/// <returns></returns>
	static float Sin(float radians) { return sinf(radians); }

	/// <summary>
	/// 余弦の値を返します。(角度はラジアン)
	/// </summary>
	/// <param name="radians"></param>
	/// <returns></returns>
	static float Cos(float radians) { return cosf(radians); }

	/// <summary>
	/// 正接の値を返します。(角度はラジアン)
	/// </summary>
	/// <param name="radians"></param>
	/// <returns></returns>
	static float Tan(float radians) { return tanf(radians); }

	/// <summary>
	/// 逆正接の値を返します。
	/// </summary>
	/// <param name="y"></param>
	/// <param name="x"></param>
	/// <returns></returns>
	static float Atan2(float y, float x) { return atan2f(y, x); }

	/// <summary>
	/// 平方根の値を返します。
	/// </summary>
	/// <param name="value"></param>
	/// <returns></returns>
	static float Sqrt(float value) { return sqrt(value); }

	/// <summary>
	/// 2つの値を交換します。
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="a"></param>
	/// <param name="b"></param>
	template<typename T>
	static constexpr void Swap(T& a, T& b) { const T temp = a; a = b; b = temp; }
};