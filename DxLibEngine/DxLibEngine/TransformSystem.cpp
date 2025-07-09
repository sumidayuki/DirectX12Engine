#include "TransformSystem.h"

const Matrix4x4& TransformSystem::GetLocalToWorldMatrix(Transform& transform) const
{
	// 各種行列を再計算
	RecalculateMatricesIfNeeded(transform);

	// ワールド変換行列の参照を返す。
	return transform.localToWorldMatrix;
}

const Matrix4x4& TransformSystem::GetWorldToLocalMatrix(Transform& transform) const
{
	// 各種行列の再計算
	RecalculateMatricesIfNeeded(transform);

	// ワールド変換行列の逆行列の参照を返す。
	return transform.warldToLocalMatrix;
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

void TransformSystem::RecalculateMatricesIfNeeded(Transform& transform) const
{
	if (transform.dirty)
	{
		transform.localMatrix.SetSRT(transform.scale, transform.rotation, transform.position);
		transform.dirty = false;
	}

	if (true)
	{
		// ワールド変換行列を計算する
		if (transform.parent != nullptr)
		{
			// 親が設定されている時
			transform.localToWorldMatrix = GetLocalToWorldMatrix(*transform.parent) * transform.localMatrix;
		}
		else
		{
			transform.localToWorldMatrix = transform.localMatrix;
		}

		transform.warldToLocalMatrix = transform.localToWorldMatrix.Inverse();
		transform.hasChanged = false;
	}
}

void TransformSystem::Start(ComponentManager& cm, World& world)
{
	m_view = std::make_unique<View<Transform>>(cm);
}

void TransformSystem::Update(ComponentManager& cm, World& world)
{
	m_view->Update();

	for (auto&& [entity, transform] : *m_view)
	{
		// entity が enabled ではないならスキップします。（処理を行う必要がないため）
		if (!entity.enabled) continue;

		// dirty が false ならスキップします。（更新を掛ける必要がないため）
		if (!transform.dirty) continue;

		// スケール・回転・位置からローカル変換行列を計算します。
		RecalculateMatricesIfNeeded(transform);
	}
}