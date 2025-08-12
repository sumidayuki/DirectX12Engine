#pragma once

/// <summary>
/// �g�����X�t�H�[����\���R���|�[�l���g�ł��B
/// �e�q�֌W�⎩���̃��[�J�����W�E���[�J����ԍ��W�E���[���h��ԍ��W�E���[���h��ԍ��W�̋t�s��������܂��B
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