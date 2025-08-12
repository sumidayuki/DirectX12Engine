#pragma once

/// <summary>
/// トランスフォームを表すコンポーネントです。
/// 親子関係や自分のローカル座標・ローカル空間座標・ワールド空間座標・ワールド空間座標の逆行列を持ちます。
/// </summary>
struct Transform
{
	Transform* parent = nullptr;

	Vector3 position = Vector3(0.0f, 0.0f, 0.0f);
	Quaternion rotation = Quaternion::identity;
	Vector3 scale = Vector3(1.0f, 1.0f, 1.0f);

	mutable Matrix4x4 localMatrix = Matrix4x4::identity;
	mutable Matrix4x4 localToWorldMatrix = Matrix4x4::identity;
	mutable Matrix4x4 worldToLocalMatrix = Matrix4x4::identity;

	mutable bool dirty = true;
	mutable bool hasChanged = true;
};