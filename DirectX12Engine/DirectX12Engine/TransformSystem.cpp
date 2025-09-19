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

Vector3 TransformSystem::TransformDirection(Transform& transform, const Vector3& direction)
{
	// ワールド変換行列を取得する
	const Matrix4x4& m = GetLocalToWorldMatrix(transform);

	Vector3 result
	(
		direction.x * m._11 + direction.y * m._21 + direction.z * m._31,
		direction.x * m._12 + direction.y * m._22 + direction.z * m._32,
		direction.x * m._13 + direction.y * m._23 + direction.z * m._33
	);

	// direction と同じノルムにして返す
	return result * (direction.Magnitude() / result.Magnitude());
}

Vector3 TransformSystem::TransformVector(Transform& transform, const Vector3& vector)
{
	// ワールド変換行列を取得する
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
	// ワールド変換行列を取得する
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
	// ワールド変換行列を取得する
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

	// 変更前と変更先の親を記憶しておく
	Transform* before = transform.parent;
	Transform* after = parent;

	// 親を変更する前のワールド空間内での位置を保存しておく。
	const Vector3 worldPosition = GetPosition(transform);

	// 既に親がいる場合は子供リストから自分を削除する
	if (transform.parent)
	{
		transform.parent->children.remove(&transform);
	}

	// 新しい親の子供リストに自分を追加する
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
	// リスト構造はランダムアクセスができないので、
	// シーケンシャルアクセスで目的のTransformを探す。
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
	// 指定された軸と角度で回転するクォータニオンを作成
	Quaternion rot = Quaternion::AngleAxis(angle, axis);

	// 位置を更新
	Vector3 dir = transform.position - point;
	dir = rot * dir; // クォータニオンで方向ベクトルを回転
	transform.position = point + dir;

	// 回転を更新
	transform.rotation = rot * transform.rotation;

	// 行列の再計算を予約
	transform.dirty = true;
}

Vector3 TransformSystem::GetPosition(Transform& transform)
{
	if (transform.parent)
	{
		// 親がいる場合はワールド位置に変換して返す
		return TransformPoint(*transform.parent, transform.position);
	}
	else
	{
		// 親がいない場合はローカル位置とワールド位置は一致する
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
	// 親のワールド行列を使って自分のワールド行列を計算
	if (transform.parent)
	{
		transform.localToWorldMatrix = transform.localMatrix * transform.parent->localToWorldMatrix;
	}
	else
	{
		transform.localToWorldMatrix = transform.localMatrix;
	}

	// 逆行列も計算
	transform.worldToLocalMatrix = transform.localToWorldMatrix.Inverse();

	// すべての子に対しても再帰的に計算を実行
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