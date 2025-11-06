#include "TransformSystem.h"

const Matrix4x4& TransformSystem::GetLocalToWorldMatrix(Transform& transform)
{
	// ワールド変換行列の参照を返す。
	return transform.localToWorldMatrix;
}

const Matrix4x4& TransformSystem::GetWorldToLocalMatrix(Transform& transform)
{
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
	if (transform.parent == (parent ? parent->entity : INVALID_ENTITY)) return;

	// 古い親から削除
	if (transform.parent != INVALID_ENTITY)
	{
		std::erase(m_hierarchy[transform.parent], transform.entity);
	}

	// 新しい親に登録
	if (parent)
	{
		m_hierarchy[parent->entity].push_back(transform.entity);
		transform.parent = parent->entity;
	}
	else
	{
		transform.parent = INVALID_ENTITY;
	}

	transform.dirty = true;
	transform.hasChanged = true;
}

void TransformSystem::UnsetParent(Transform& transform)
{
	if (transform.parent == INVALID_ENTITY) return;

	std::erase(m_hierarchy[transform.parent], transform.entity);

	transform.parent = INVALID_ENTITY;
	transform.dirty = true;
	transform.hasChanged = true;
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
	auto it = m_hierarchy.find(transform->entity);
	return (it != m_hierarchy.end()) ? (int)it->second.size() : 0;
}

Transform* TransformSystem::GetChild(Transform* transform, int index) 
{
	auto it = m_hierarchy.find(transform->entity);
	if (it == m_hierarchy.end() || index >= (int)it->second.size()) return nullptr;

	Entity childEntity = it->second[index];
	return SceneManager::GetCurrentScene()->GetWorld().GetComponent<Transform>(childEntity);
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

void TransformSystem::BuildHierarchy(ComponentManager& cm)
{
	m_hierarchy.clear();
	m_roots.clear();

	auto& entities = m_transformStorage->GetEntities();
	for (Entity e : entities)
	{
		Transform* t = m_transformStorage->Get(e);
		if (!t) continue;

		if (t->parent == INVALID_ENTITY)
		{
			m_roots.push_back(e);
		}
		else
		{
			m_hierarchy[t->parent].push_back(e);
		}
	}
}

void TransformSystem::TopologicalSort()
{
	m_sortedEntities.clear();
	std::unordered_set<Entity> visited;

	for (Entity root : m_roots)
	{
		Visit(root, visited);
	}
}

void TransformSystem::Visit(Entity entity, std::unordered_set<Entity>& visited)
{
	if (visited.contains(entity)) return;
	visited.insert(entity);

	auto it = m_hierarchy.find(entity);
	if (it != m_hierarchy.end())
	{
		for (Entity child : it->second)
		{
			Visit(child, visited);
		}
	}

	m_sortedEntities.push_back(entity);
}

void TransformSystem::RecalculateMatrices(World& world)
{
	for (Entity e : m_sortedEntities)
	{
		Transform* transform = m_transformStorage->Get(e);
		if (!transform) continue;

		if (transform->dirty)
		{
			transform->localMatrix.SetSRT(transform->scale, transform->rotation, transform->position);
			transform->dirty = false;
		}

		if (transform->parent != INVALID_ENTITY)
		{
			Transform* parent = m_transformStorage->Get(transform->parent);
			transform->localToWorldMatrix = transform->localMatrix * parent->localToWorldMatrix;
		}
		else
		{
			transform->localToWorldMatrix = transform->localMatrix;
		}

		transform->worldToLocalMatrix = transform->localToWorldMatrix.Inverse();
		transform->hasChanged = false;
	}
}

void TransformSystem::Start(ComponentManager& cm, World& world)
{
	m_transformStorage = cm.GetStorage<Transform>();
	BuildHierarchy(cm);
	TopologicalSort();
}

void TransformSystem::Update(ComponentManager& cm, World& world)
{
	if (!m_transformStorage) return;

	BuildHierarchy(cm);
	TopologicalSort();
	RecalculateMatrices(world);
}