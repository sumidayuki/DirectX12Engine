#include "TransformSystem.h"

const Matrix4x4& TransformSystem::GetLocalToWorldMatrix(Transform& transform)
{
	// 各種行列を再計算
	RecalculateMatricesIfNeeded(transform);

	// ワールド変換行列の参照を返す。
	return transform.localToWorldMatrix;
}

const Matrix4x4& TransformSystem::GetWorldToLocalMatrix(Transform& transform)
{
	// 各種行列の再計算
	RecalculateMatricesIfNeeded(transform);

	// ワールド変換行列の逆行列の参照を返す。
	return transform.worldToLocalMatrix;
}

void TransformSystem::SetParent(Transform& transform, Transform* parent)
{
	// 既存の親子関係を解除
	if (transform.parent != nullptr) {
		UnsetParent(transform);
	}

	transform.parent = parent;
	if (parent != nullptr) {
		parent->children.push_back(&transform);
	}

	transform.dirty = true;
}

void TransformSystem::UnsetParent(Transform& transform)
{
	if (transform.parent != nullptr) {
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

void TransformSystem::SetLocalRotation(Transform& transform, const Quaternion& localRotation)
{
	transform.rotation = localRotation;
	SetDirtyRecursively(transform);
}

void TransformSystem::SetLocalPosition(Transform& transform, const Vector3& localPosition)
{
	transform.position = localPosition;
	SetDirtyRecursively(transform);
}

void TransformSystem::SetLocalPosition(Transform& transform, float x, float y, float z)
{
	transform.position.x = x;
	transform.position.y = y;
	transform.position.z = z;
	SetDirtyRecursively(transform);
}

void TransformSystem::RotateAround(Transform& transform, Vector3 point, Vector3 axis, float angle)
{
	// 指定された軸と角度で回転するクォータニオンを作成
	Quaternion rot = Quaternion::AngleAxis(angle, axis);

	// 位置を更新
	Vector3 dir = transform.position - point;
	dir = rot * dir; // クォータニオンで方向ベクトルを回転
	transform.position = point + dir;

	// 回転を更新
	transform.rotation = rot * transform.rotation;

	// 行列の再計算を予約
	SetDirtyRecursively(transform);
}

void TransformSystem::Translate(Transform& transform, const Vector3& translation)
{
	transform.position = transform.position + translation;
	SetDirtyRecursively(transform);
}

void TransformSystem::Rotate(Transform& transform, const Vector3 axis, float angle)
{
	const Quaternion q = Quaternion::AngleAxis(angle, axis);

	transform.rotation = q * transform.rotation;
	SetDirtyRecursively(transform);
}

void TransformSystem::SetDirtyRecursively(Transform& transform)
{
	transform.dirty = true;

	if (transform.children.empty())
	{
		return;
	}

	for (auto* child : transform.children)
	{
		SetDirtyRecursively(*child);
	}
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
		// entity が enabled ではないならスキップします。（処理を行う必要がないため）
		if (!entity.enabled) continue;

		// dirty が false ならスキップします。（更新を掛ける必要がないため）
		if (!transform.dirty) continue;

		// スケール・回転・位置からローカル変換行列を計算します。
		RecalculateMatricesIfNeeded(transform);
	}
}