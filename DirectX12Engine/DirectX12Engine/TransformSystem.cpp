#include "TransformSystem.h"

const Matrix4x4& TransformSystem::GetLocalToWorldMatrix(Transform& transform)
{
	RecalculateMatricesRecursive(SceneManager::GetCurrentScene()->GetWorld(), transform);

	// ワールド変換行列の参照を返す。
	return transform.localToWorldMatrix;
}

const Matrix4x4& TransformSystem::GetWorldToLocalMatrix(Transform& transform)
{
	// 逆行列が古い場合のみ計算を実行
	if (transform.inverseDirty)
	{
		transform.worldToLocalMatrix = transform.localToWorldMatrix.Inverse();
		transform.inverseDirty = false;
	}

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
	// 逆行列を取得（遅延計算が実行される可能性がある）
	const Matrix4x4& m = GetWorldToLocalMatrix(transform);

	Vector3 result
	(
		direction.x * m._11 + direction.y * m._21 + direction.z * m._31,
		direction.x * m._12 + direction.y * m._22 + direction.z * m._32,
		direction.x * m._13 + direction.y * m._23 + direction.z * m._33
	);

	// direction と同じノルムにして返す
	return result * (direction.Magnitude() / result.Magnitude());
}

Vector3 TransformSystem::InverseTransformVector(Transform& transform, const Vector3& vector)
{
	// 逆行列を取得
	const Matrix4x4& m = GetWorldToLocalMatrix(transform);

	return Vector3
	(
		vector.x * m._11 + vector.y * m._21 + vector.z * m._31,
		vector.x * m._12 + vector.y * m._22 + vector.z * m._32,
		vector.x * m._13 + vector.y * m._23 + vector.z * m._33
	);
}

Vector3 TransformSystem::InverseTransformPoint(Transform& transform, const Vector3& position)
{
	// 逆行列を取得
	const Matrix4x4& m = GetWorldToLocalMatrix(transform);

	// 逆行列を適用
	return Vector3
	(
		position.x * m._11 + position.y * m._21 + position.z * m._31 + 1.0f * m._41,
		position.x * m._12 + position.y * m._22 + position.z * m._32 + 1.0f * m._42,
		position.x * m._13 + position.y * m._23 + position.z * m._33 + 1.0f * m._43
	);
}

void TransformSystem::SetParent(Transform& transform, Transform* parent)
{
	World& world = SceneManager::GetCurrentScene()->GetWorld();

	if (transform.parent == (parent ? parent->entity : INVALID_ENTITY)) return;

	// 古い親から子を削除
	if (transform.parent != INVALID_ENTITY)
	{
		Transform* oldParent = world.GetComponent<Transform>(transform.parent);
		if (oldParent)
		{
			// 古い親の children リストから自分を削除
			std::erase(oldParent->children, transform.entity);
		}
	}

	// 新しい親に設定
	if (parent)
	{
		// 新しい親の children リストに自分を追加
		parent->children.push_back(transform.entity);
		transform.parent = parent->entity;
	}
	else
	{
		transform.parent = INVALID_ENTITY;
	}

	// ダーティフラグを設定
	transform.dirty = true;
}

void TransformSystem::UnsetParent(Transform& transform)
{
	World& world = SceneManager::GetCurrentScene()->GetWorld();
	if (transform.parent == INVALID_ENTITY) return;

	// 古い親から子を削除
	Transform* oldParent = world.GetComponent<Transform>(transform.parent);
	if (oldParent)
	{
		std::erase(oldParent->children, transform.entity);
	}

	// 親を解除
	transform.parent = INVALID_ENTITY;

	// ダーティフラグを設定
	transform.dirty = true;
}

Transform* TransformSystem::GetRoot(Transform& transform)
{
	World& world = SceneManager::GetCurrentScene()->GetWorld();

	Transform* root = &transform;
	
	while (root->parent != INVALID_ENTITY)
	{
		root = world.GetComponent<Transform>(root->parent);
	}

	return root;
}

int TransformSystem::GetChildCount(Transform* transform)
{
	return transform->children.size();
}

Transform* TransformSystem::GetChild(Transform* transform, int index) 
{
	if (index >= transform->children.size())
	{
		return nullptr;
	}
	return 	SceneManager::GetCurrentScene()->GetWorld().GetComponent<Transform>(transform->children[index]);
}

Transform* TransformSystem::FindChild(Transform* transform, const std::string& name)
{
	if (transform->entity.name == name)
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
	World& world = SceneManager::GetCurrentScene()->GetWorld();

	if (transform.parent != INVALID_ENTITY)
	{
		// 親がいる場合はワールド位置に変換して返す
		return TransformPoint(*world.GetComponent<Transform>(transform.parent), transform.position);
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

void TransformSystem::RecalculateMatricesRecursive(World& world, Transform& transform)
{
	bool parentChanged = false;

	if (transform.parent != INVALID_ENTITY)
	{
		Transform* parent = world.GetComponent<Transform>(transform.parent);
		if (parent)
		{
			// 親のワールド行列がこのフレームで変更されたか
			parentChanged = parent->hasChanged;
		}
		else
		{
			// 親エンティティが破壊されている場合、親なしに設定し、dirtyフラグを立てる
			transform.parent = INVALID_ENTITY;
			transform.dirty = true;
		}
	}

	if (transform.dirty || parentChanged)
	{
		// ローカル行列の更新（ローカルプロパティ変更時のみ）
		if (transform.dirty)
		{
			transform.localMatrix.SetSRT(transform.scale, transform.rotation, transform.position);
			transform.dirty = false;
		}

		// ワールド行列の計算
		if (transform.parent != INVALID_ENTITY)
		{
			Transform* parent = world.GetComponent<Transform>(transform.parent);
			transform.localToWorldMatrix = transform.localMatrix * parent->localToWorldMatrix;
		}
		else
		{
			transform.localToWorldMatrix = transform.localMatrix;
		}

		// 自身のワールド行列が更新されたので、逆行列は古くなる
		transform.inverseDirty = true;

		// 自身のワールド行列が変更されたことをマークし、子に伝播させる
		transform.hasChanged = true;
	}
	else
	{
		// 更新がない場合、hasChangedをリセット
		transform.hasChanged = false;
	}

	if (transform.hasChanged)
	{
		for (Entity child : transform.children)
		{
			Transform* childTransform = world.GetComponent<Transform>(child);
			if (childTransform)
			{
				RecalculateMatricesRecursive(world, *childTransform);
			}
		}
	}
}

void TransformSystem::Start(World& world)
{

}

void TransformSystem::Update(World& world)
{
	View<Transform> view(world);

	for (auto [entity, transform] : view)
	{
		transform.hasChanged = false;
	}

	for (auto [entity, transform] : view)
	{
		if (transform.parent == INVALID_ENTITY)
		{
			// 親がいない場合（ルート）は、ダーティなら再帰的な計算を開始
			if (transform.dirty)
			{
				RecalculateMatricesRecursive(world, transform);
			}
		}
	}
}