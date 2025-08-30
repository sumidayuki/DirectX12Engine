#pragma once

// 授業で作りました。
// Swap関数はfloat型でしかできなかったものをテンプレート化しました。

/// <summary>
/// 数学を管理するクラスです。
/// </summary>
class Mathf
{
public:
	// 円周率π (float型の有効桁数は7桁)
	static constexpr float PI = 3.1415926f;

	// πの2倍の値を表します。
	static constexpr float TwoPI = 6.283185307f;

	// πを2で割った値(π/2)を表します。
	static constexpr float PIOver2 = 1.570796327f;

	// πを4で割った値(π/4)を表します。
	static constexpr float PIOver4 = 0.785398163f;

	// 自然対数の底e(ネイピア数)を表します。
	static constexpr float E = 2.7182818f;

	// 2を底とするeの対数を表します。
	static constexpr float Log2E = 1.4426950408889634f;

	// 10を底とするeの対数を表します。
	static constexpr float Log10E = 0.4342944819032518f;

	// 度をラジアンに変換するための係数
	static constexpr float Deg2Rad = Mathf::PI / 180.0f;

	// ラジアンを度に変換するための係数
	static constexpr float Rad2Deg = 180.0f / Mathf::PI;

	// 計算機イプシロン (極めて 0 に近い小さな値)
	static constexpr float Epsilon = std::numeric_limits<float>::epsilon();

	// 正の無限大を表します。
	static constexpr float Infinity = std::numeric_limits<float>::infinity();

	// 負の無限大を表します。
	static constexpr float NegativeInfinity = -Infinity;

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

	// valueを[minValue, maxValue]の範囲に制限した値を返します。
	static constexpr float Clamp(float value, float minValue, float maxValue) { return (value < minValue) ? minValue : ((value > maxValue) ? maxValue : value); }

	// [a, b]の間でパラメーター t による線形補間します。t は[0, 1]に制限されます。
	//「Lerp」は線形補間(Linear Interpolate)の略です。
	static constexpr float Lerp(float a, float b, float t) { return LerpUnclamped(a, b, Clamp(t, 0, 1)); }


	/// <summary>
	/// [a, b]の間でパラメーター t による線形補間します。t に制限はありません。
	/// </summary>
	/// <param name="a"></param>
	/// <param name="b"></param>
	/// <param name="t"></param>
	/// <returns></returns>
	static constexpr float LerpUnclamped(float a, float b, float t) { return a + (b - a) * t; }
};