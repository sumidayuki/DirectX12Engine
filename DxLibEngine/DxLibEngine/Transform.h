#pragma once

struct Transform
{
	// x, y �͉�ʂ̍��W z �͕`�揇�i�~���j��\���܂��B
	Vector3 position = Vector3(0.0f, 0.0f, 0.0f);
	Quaternion rotation = Quaternion::identity;
	Vector3 scale = Vector3(1.0f, 1.0f, 1.0f);

	Entity parent = NullEntity;
	Matrix4x4 localMatrix = Matrix4x4::identity;
	Matrix4x4 worldMatrix = Matrix4x4::identity;

	bool dirty = true;
};