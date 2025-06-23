#pragma once

/// <summary>
/// 四元数を表すクラスです。
/// </summary>
class Quaternion
{
public:
	// 恒等四元数 (回転なし)
	static const Quaternion identity;

public:
	union
	{
		// 成分表現
		struct
		{
			float x;
			float y;
			float z;
			float w;
		};

		// （虚数ベクトル、実数）表現
		struct
		{
			Vector3 imaginary;
			float real;
		};

		// 配列表現
		int components[4];
	};

public:
	// デフォルトコンストラクタ
	Quaternion() = default;

	/// <summary>
	/// 新しいクォータニオン(x, y, z, w)を作成します。
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <param name="z"></param>
	/// <param name="w"></param>
	Quaternion(float x, float y, float z, float w);

	/// <summary>
	/// Quaternion型からXMVECTOR型に変換します。
	/// </summary>
	/// <returns></returns>
	[[nodiscard]] DirectX::XMVECTOR ToXMVECTOR() const;

	/// <summary>
	/// 回転軸 axis の周りを angle 度だけ回転させるクォータニオンを作成します。 (単位はディグリー)
	/// </summary>
	/// <param name="angle"></param>
	/// <param name="axis"></param>
	/// <returns></returns>
	[[nodiscard]] static Quaternion AngleAxis(float angle, const Vector3& axis);
};

[[nodiscard]] Quaternion operator *(const Quaternion& lhs, const Quaternion& rhs) noexcept;