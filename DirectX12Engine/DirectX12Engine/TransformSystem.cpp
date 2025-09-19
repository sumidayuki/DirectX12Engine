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

Vector3 TransformSystem::TransformDirection(Transform& transform, const Vector3& direction)
{
	// ���[���h�ϊ��s����擾����
	const Matrix4x4& m = GetLocalToWorldMatrix(transform);

	Vector3 result
	(
		direction.x * m._11 + direction.y * m._21 + direction.z * m._31,
		direction.x * m._12 + direction.y * m._22 + direction.z * m._32,
		direction.x * m._13 + direction.y * m._23 + direction.z * m._33
	);

	// direction �Ɠ����m�����ɂ��ĕԂ�
	return result * (direction.Magnitude() / result.Magnitude());
}

Vector3 TransformSystem::TransformVector(Transform& transform, const Vector3& vector)
{
	// ���[���h�ϊ��s����擾����
	const Matrix4x4& m = GetLocalToWorldMatrix(transform);

	return Vector3
	(
		vector.x * m._11 + vector.y * m._21 + vector.z * m._31,
		vector.x * m._12 + vector.y * m._22 + vector.z * m._32,
		vector.x * m._13 + vector.y * m._23 + vector.z * m._33
	);
}

Vector3 TransformSystem::TransformPoint(Transform& transform, const Vector3& position)
{
	// ���[���h�ϊ��s����擾����
	const Matrix4x4& m = GetLocalToWorldMatrix(transform);

	return Vector3
	(
		position.x * m._11 + position.y * m._21 + position.z * m._31 + 1.0f * m._41,
		position.x * m._12 + position.y * m._22 + position.z * m._32 + 1.0f * m._42,
		position.x * m._13 + position.y * m._23 + position.z * m._33 + 1.0f * m._43
	);
}

Vector3 TransformSystem::InverseTransformDirection(Transform& transform, const Vector3& direction)
{
	return Vector3();
}

Vector3 TransformSystem::InverseTransformVector(Transform& transform, const Vector3& vector)
{
	return Vector3();
}

Vector3 TransformSystem::InverseTransformPoint(Transform& transform, const Vector3& position)
{
	// ���[���h�ϊ��s����擾����
	const Matrix4x4& m = GetLocalToWorldMatrix(transform);

	return Vector3
	(
		position.x * m._11 + position.y * m._21 + position.z * m._31 + 1.0f * m._41,
		position.x * m._12 + position.y * m._22 + position.z * m._32 + 1.0f * m._42,
		position.x * m._13 + position.y * m._23 + position.z * m._33 + 1.0f * m._43
	);
}

void TransformSystem::SetParent(Transform& transform, Transform* parent)
{
	if (parent == transform.parent)
	{
		return;
	}

	// �ύX�O�ƕύX��̐e���L�����Ă���
	Transform* before = transform.parent;
	Transform* after = parent;

	// �e��ύX����O�̃��[���h��ԓ��ł̈ʒu��ۑ����Ă����B
	const Vector3 worldPosition = GetPosition(transform);

	// ���ɐe������ꍇ�͎q�����X�g���玩�����폜����
	if (transform.parent)
	{
		transform.parent->children.remove(&transform);
	}

	// �V�����e�̎q�����X�g�Ɏ�����ǉ�����
	if (parent)
	{
		parent->children.push_back(&transform);
	}

	if (true  /* worldPositonStays */ )
	{
		if (parent)
		{
			SetLocalPosition(transform, InverseTransformPoint(*parent, worldPosition));
		}
		else
		{
			SetLocalPosition(transform, worldPosition);
		}
	}

	transform.parent = parent;

	transform.hasChanged = true;
}

void TransformSystem::UnsetParent(Transform& transform)
{
	if (transform.parent != nullptr) 
	{
		transform.parent->children.remove(&transform);
		transform.parent = nullptr;
		transform.dirty = true;
	}
}

Transform* TransformSystem::GetRoot(Transform& transform)
{
	Transform* root = &transform;
	
	while (root->parent)
	{
		root = root->parent;
	}

	return root;
}

Transform* TransformSystem::GetChild(Transform* transform, int index) 
{
	// ���X�g�\���̓����_���A�N�Z�X���ł��Ȃ��̂ŁA
	// �V�[�P���V�����A�N�Z�X�ŖړI��Transform��T���B
	auto ite = transform->children.begin();
	for (int i = 0; i < index; i++)
	{
		++ite;
	}
	return *ite;
}

Transform* TransformSystem::FindChild(Transform* transform, const std::string& name)
{
	if (transform->entity->name == name)
	{
		return transform;
	}

	const int childCount = GetChildCount(transform);
	for (int i = 0; i < childCount; ++i)
	{
		Transform* childTransform = GetChild(transform, i);
		Transform* foundTransform = FindChild(childTransform, name);
		if (foundTransform != nullptr)
		{
			return foundTransform;
		}
	}

	return nullptr;
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

void TransformSystem::RotateAround(Transform& transform, Vector3 point, Vector3 axis, float angle)
{
	// �w�肳�ꂽ���Ɗp�x�ŉ�]����N�H�[�^�j�I�����쐬
	Quaternion rot = Quaternion::AngleAxis(angle, axis);

	// �ʒu���X�V
	Vector3 dir = transform.position - point;
	dir = rot * dir; // �N�H�[�^�j�I���ŕ����x�N�g������]
	transform.position = point + dir;

	// ��]���X�V
	transform.rotation = rot * transform.rotation;

	// �s��̍Čv�Z��\��
	transform.dirty = true;
}

Vector3 TransformSystem::GetPosition(Transform& transform)
{
	if (transform.parent)
	{
		// �e������ꍇ�̓��[���h�ʒu�ɕϊ����ĕԂ�
		return TransformPoint(*transform.parent, transform.position);
	}
	else
	{
		// �e�����Ȃ��ꍇ�̓��[�J���ʒu�ƃ��[���h�ʒu�͈�v����
		return transform.position;
	}
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
	// �e�̃��[���h�s����g���Ď����̃��[���h�s����v�Z
	if (transform.parent)
	{
		transform.localToWorldMatrix = transform.localMatrix * transform.parent->localToWorldMatrix;
	}
	else
	{
		transform.localToWorldMatrix = transform.localMatrix;
	}

	// �t�s����v�Z
	transform.worldToLocalMatrix = transform.localToWorldMatrix.Inverse();

	// ���ׂĂ̎q�ɑ΂��Ă��ċA�I�Ɍv�Z�����s
	for (auto* child : transform.children)
	{
		RecalculateMatricesIfNeeded(*child);
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
		if (transform.dirty)
		{
			transform.localMatrix.SetSRT(transform.scale, transform.rotation, transform.position);
			transform.dirty = false;
		}

		if (true)
		{
			if (transform.parent)
			{
				transform.localToWorldMatrix = transform.localMatrix * GetLocalToWorldMatrix(*transform.parent);
			}
			else
			{
				transform.localToWorldMatrix = transform.localMatrix;
			}

			transform.worldToLocalMatrix = transform.localToWorldMatrix.Inverse();
			transform.hasChanged = false;
		}
	}
}