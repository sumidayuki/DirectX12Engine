#pragma once

struct Transform
{
	// x, y は画面の座標 z は描画順（降順）を表します。
	Vector3 position = Vector3(0.0f, 0.0f, 0.0f);
	Quaternion rotation = Quaternion::identity;
	Vector3 scale = Vector3(1.0f, 1.0f, 1.0f);

	Entity parent = NullEntity;
	Matrix4x4 localMatrix = Matrix4x4::identity;
	Matrix4x4 worldMatrix = Matrix4x4::identity;

	bool dirty = true;
};