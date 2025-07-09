#pragma once

/// <summary>
/// Transformの管理を行うシステムです。
/// Transformコンポーネントの変更をMatrix等に反映します。
/// </summary>
class TransformSystem : public System
{
private:
	std::unique_ptr<View<Transform>> m_view;

	std::unordered_map<Entity, std::vector<Entity>> m_hierarhy;

public:
	void SetLocalRotation(Transform& transform, const Quaternion& localRotation);

	void SetLocalPosition(Transform& transform, const Vector3& localPosition);

	void SetLocalPosition(Transform& transform, float x, float y, float z);


	const Matrix4x4& GetLocalToWorldMatrix(Transform& transform) const;

	const Matrix4x4& GetWorldToLocalMatrix(Transform& transform) const;

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
	void RecalculateMatricesIfNeeded(Transform& transform) const;

public:
	void Start(ComponentManager& cm, World& world) override;

	void Update(ComponentManager& cm, World& world) override;
};