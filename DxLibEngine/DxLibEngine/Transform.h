#pragma once

struct Transform
{
	Transform* parent = nullptr;

	Vector3 position = Vector3(0.0f, 0.0f, 0.0f);
	Quaternion rotation = Quaternion::identity;
	Vector3 scale = Vector3(1.0f, 1.0f, 1.0f);

	mutable Matrix4x4 localMatrix = Matrix4x4::identity;
	mutable Matrix4x4 localToWorldMatrix = Matrix4x4::identity;
	mutable Matrix4x4 warldToLocalMatrix = Matrix4x4::identity;

	mutable bool dirty = true;
	mutable bool hasChanged = true;
};