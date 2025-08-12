#include "TransformSystem.h"

const Matrix4x4& TransformSystem::GetLocalToWorldMatrix(Transform& transform)
{
	// �e��s����Čv�Z
	RecalculateMatricesIfNeeded(transform);

	// ���[���h�ϊ��s��̎Q�Ƃ�Ԃ��B
	return transform.localToWorldMatrix;
}

const Matrix4x4& TransformSystem::GetWorldToLocalMatrix(Transform& transform)
{
	// �e��s��̍Čv�Z
	RecalculateMatricesIfNeeded(transform);

	// ���[���h�ϊ��s��̋t�s��̎Q�Ƃ�Ԃ��B
	return transform.worldToLocalMatrix;
}

void TransformSystem::SetLocalRotation(Transform& transform, const Quaternion& localRotation)
{
	transform.rotation = localRotation;
	transform.dirty = true;
}

void TransformSystem::SetLocalPosition(Transform& transform, const Vector3& localPosition)
{
	transform.position = localPosition;
	transform.dirty = true;
}

void TransformSystem::SetLocalPosition(Transform& transform, float x, float y, float z)
{
	transform.position.x = x;
	transform.position.y = y;
	transform.position.z = z;
	transform.dirty = true;
}

void TransformSystem::Translate(Transform& transform, const Vector3& translation)
{
	transform.position = transform.position + translation;
	transform.dirty = true;
}

void TransformSystem::Rotate(Transform& transform, const Vector3 axis, float angle)
{
	const Quaternion q = Quaternion::AngleAxis(angle, axis);

	transform.rotation = q * transform.rotation;
	transform.dirty = true;
}

void TransformSystem::RecalculateMatricesIfNeeded(Transform& transform)
{
	if (transform.dirty)
	{
		transform.localMatrix.SetSRT(transform.scale, transform.rotation, transform.position);
		transform.dirty = false;
	}

	if (true)
	{
		// ���[���h�ϊ��s����v�Z����
		if (transform.parent != nullptr)
		{
			// �e���ݒ肳��Ă��鎞
			transform.localToWorldMatrix = GetLocalToWorldMatrix(*transform.parent) * transform.localMatrix;
		}
		else
		{
			transform.localToWorldMatrix = transform.localMatrix;
		}

		transform.worldToLocalMatrix = transform.localToWorldMatrix.Inverse();
		transform.hasChanged = false;
	}
}

void TransformSystem::Start(ComponentManager& cm, World& world)
{
}

void TransformSystem::Update(ComponentManager& cm, World& world)
{
	View<Transform> view(cm);

	for (auto [entity, transform] : view)
	{
		// entity �� enabled �ł͂Ȃ��Ȃ�X�L�b�v���܂��B�i�������s���K�v���Ȃ����߁j
		if (!entity.enabled) continue;

		// dirty �� false �Ȃ�X�L�b�v���܂��B�i�X�V���|����K�v���Ȃ����߁j
		if (!transform.dirty) continue;

		// �X�P�[���E��]�E�ʒu���烍�[�J���ϊ��s����v�Z���܂��B
		RecalculateMatricesIfNeeded(transform);
	}
}