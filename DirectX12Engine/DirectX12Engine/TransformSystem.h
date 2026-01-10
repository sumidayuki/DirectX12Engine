#pragma once

class World;

/// <summary>
/// Transformの管理を行うシステムです。
/// Transformコンポーネントの変更をMatrix等に反映します。
/// </summary>
class TransformSystem : public System, public Singleton<TransformSystem>
{
	friend class Singleton<TransformSystem>;

public:
	/// <summary>
	/// 指定された transform に親を設定します。
	/// 親子関係を設定すると、親のトランスフォームに連動して移動・回転・スケールされます。
	/// </summary>
	/// <param name="transform">子にするtransform</param>
	/// <param name="parent">親にするtransform</param>
	void SetParent(Transform& transform, Transform* parent);

	/// <summary>
	/// エンティティが削除される際に呼び出され、親子関係の解除と子の再帰削除を行います。
	/// </summary>
	void OnEntityDestroyed(World& world, Entity entity);

	/// <summary>
	/// 親子関係を解除します。
	/// </summary>
	/// <param name="transform">親から切り離したいtransform</param>
	void UnsetParent(Transform& transform);

	Transform* GetRoot(Transform& transform);

	// 子Transformの個数を取得します。
	int GetChildCount(Transform* transform);

	// index番目の子Transformを取得します。
	Transform* GetChild(Transform* transform, int index);

	Transform* FindChild(Transform* transform, const std::string& name);

	void SetLocalRotation(Transform& transform, const Quaternion& localRotation);

	void SetLocalPosition(Transform& transform, const Vector3& localPosition);

	void SetLocalPosition(Transform& transform, float x, float y, float z);

	void RotateAround(Transform& transform, Vector3 point, Vector3 axis, float angle);

	Vector3 GetPosition(Transform& transform);

	const Matrix4x4& GetLocalToWorldMatrix(Transform& transform);

	const Matrix4x4& GetWorldToLocalMatrix(Transform& transform);

	// ローカル空間からワールド空間へ direction を変換します。
	// この変換は「スケール」「位置」の影響を受けません。
	// 返されるベクトルは direction と同じ長さになります。
	Vector3 TransformDirection(Transform& transform, const Vector3& direction);
	 
	// ローカル空間からワールド空間へ vector を変換します。
	// この変換は「スケール」の影響を受けますが「位置」の影響は受けません。
	// 返されるベクトルの長さは、vector とは異なる場合があります。
	Vector3 TransformVector(Transform& transform, const Vector3& vector);

	// ローカル空間からワールド空間へ position を変換します。
	// この変換は「スケール」「回転」「位置」の影響を受けます。
	Vector3 TransformPoint(Transform& transform, const Vector3& position);

	// ワールド空間からローカル空間へ direction を変換します。
	// この変換は「スケール」「位置」の影響を受けません。
	// 返されるベクトルは direction と同じ長さになります。
	Vector3 InverseTransformDirection(Transform& transform, const Vector3& direction);

	// ワールド空間からローカル空間へ vector を変換します。
	// この変換は「スケール」の影響を受けますが「位置」の影響は受けません。
	// 返されるベクトルの長さは、vector とは異なる場合があります。
	Vector3 InverseTransformVector(Transform& transform, const Vector3& vector);

	// ワールド空間からローカル空間へ position を変換します。
	// この変換は「スケール」「回転」「位置」の影響を受けます。
	Vector3 InverseTransformPoint(Transform& transform, const Vector3& position);

	/// <summary>
	/// 現在の位置から指定した分だけ移動します。
	/// </summary>
	/// <param name="transform">移動させたいtransform</param>
	/// <param name="translation">移動量</param>
	void Translate(Transform& transform, const Vector3& translation);

	/// <summary>
	/// 指定した回転軸に指定の角度だけ回転させます。
	/// </summary>
	/// <param name="transform">回転させたいtransform</param>
	/// <param name="axis">回転軸</param>
	/// <param name="angle">角度</param>
	void Rotate(Transform& transform, const Vector3 axis, float angle);

private:


	void UpdateLocalMatrix(Transform& transform);
	void UpdateAllDirtyTransforms(World& world);

public:
	void Start(World& world) override;

	void Update(World& world) override;
};