#pragma once

/// <summary>
/// Transformの管理を行うシステムです。
/// Transformコンポーネントの変更をMatrix等に反映します。
/// </summary>
class TransformSystem : public System
{
private:
	// 親をキーとして、その直接の子エンティティのリストを保持するマップ
	std::unordered_map<Entity, std::vector<Entity>> m_hierarchy;

	// ルートとなるエンティティ（親がいないもの）のリスト
	std::vector<Entity> m_roots;

public:
	static void SetLocalRotation(Transform& transform, const Quaternion& localRotation);

	static void SetLocalPosition(Transform& transform, const Vector3& localPosition);

	static void SetLocalPosition(Transform& transform, float x, float y, float z);


	static const Matrix4x4& GetLocalToWorldMatrix(Transform& transform);

	static const Matrix4x4& GetWorldToLocalMatrix(Transform& transform);

	/// <summary>
	/// 現在の位置から指定した分だけ移動します。
	/// </summary>
	/// <param name="transform">移動させたいtransform</param>
	/// <param name="translation">移動量</param>
	static void Translate(Transform& transform, const Vector3& translation);

	/// <summary>
	/// 指定した回転軸に指定の角度だけ回転させます。
	/// </summary>
	/// <param name="transform">回転させたいtransform</param>
	/// <param name="axis">回転軸</param>
	/// <param name="angle">角度</param>
	static void Rotate(Transform& transform, const Vector3 axis, float angle);

private:
	static void RecalculateMatricesIfNeeded(Transform& transform);

public:
	void Start(ComponentManager& cm, World& world) override;

	void Update(ComponentManager& cm, World& world) override;
};