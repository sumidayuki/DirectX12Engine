#pragma once

// 授業で作りました。

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

	// XMVECTOR型の値で初期化された新しいクォータニオンを作成します。
	Quaternion(const DirectX::XMVECTOR& xmquat);

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

	// オイラー角表現でクォータニオンを作成します。 (単位はディグリー)
	//	Unityの場合 : Z-Y-Xの順で回転
	//	DirectXの場合 : Z-X-Yの順で回転
	[[nodiscard]] static Quaternion Euler(float x, float y, float z);

	// オイラー角表現でクォータニオンを作成します。 (単位はディグリー)
	//  Unityの場合 : Z-Y-Xの順で回転
	//	DirectXの場合 : Z-X-Yの順で回転
	[[nodiscard]] static Quaternion Euler(const Vector3& euler);

	// 指定された forward と upwards 方向に回転します。
	[[nodiscard]] static Quaternion LookRotation(const Vector3& forward, const Vector3& upwards = Vector3::up);

	// 四元数 a と b の間をパラメーター t を使って球面線形補間した後、その結果を正規化した四元数を返します。
	// パラメーター t は[0,1]の範囲にクランプされます。
	[[nodiscard]] static Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t);

};

[[nodiscard]] Quaternion operator *(const Quaternion& lhs, const Quaternion& rhs) noexcept;

[[nodescard]] Vector3 operator *(const Quaternion& q, const Vector3& v);