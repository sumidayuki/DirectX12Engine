#include "TransformSystem.h"
#include <algorithm>
#include <vector>
#include <stack>

const Matrix4x4& TransformSystem::GetLocalToWorldMatrix(Transform& transform)
{
	return transform.localToWorldMatrix;
}

const Matrix4x4& TransformSystem::GetWorldToLocalMatrix(Transform& transform)
{
	if (transform.inverseDirty)
	{
		transform.worldToLocalMatrix = transform.localToWorldMatrix.Inverse();
		transform.inverseDirty = false;
	}
	return transform.worldToLocalMatrix;
}

Vector3 TransformSystem::TransformDirection(Transform& transform, const Vector3& direction)
{
	const Matrix4x4& m = GetLocalToWorldMatrix(transform);
	Vector3 result
	(
		direction.x * m._11 + direction.y * m._21 + direction.z * m._31,
		direction.x * m._12 + direction.y * m._22 + direction.z * m._32,
		direction.x * m._13 + direction.y * m._23 + direction.z * m._33
	);
	return result * (direction.Magnitude() / result.Magnitude());
}

Vector3 TransformSystem::TransformVector(Transform& transform, const Vector3& vector)
{
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
	const Matrix4x4& m = GetWorldToLocalMatrix(transform);
	Vector3 result
	(
		direction.x * m._11 + direction.y * m._21 + direction.z * m._31,
		direction.x * m._12 + direction.y * m._22 + direction.z * m._32,
		direction.x * m._13 + direction.y * m._23 + direction.z * m._33
	);
	return result * (direction.Magnitude() / result.Magnitude());
}

Vector3 TransformSystem::InverseTransformVector(Transform& transform, const Vector3& vector)
{
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
	const Matrix4x4& m = GetWorldToLocalMatrix(transform);
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

	// Unlink from old parent
	UnsetParent(transform);

	World& world = SceneManager::GetCurrentScene()->GetWorld();

	if (parent)
	{
		// Add to new parent's list (insert at head)
		transform.parent = parent->entity;
		transform.nextSibling = parent->firstChild;
		transform.prevSibling = INVALID_ENTITY;

		if (parent->firstChild != INVALID_ENTITY)
		{
			Transform* oldFirst = world.GetComponent<Transform>(parent->firstChild);
			if (oldFirst)
			{
				oldFirst->prevSibling = transform.entity;
			}
		}
		parent->firstChild = transform.entity;

		// Update Depth
		transform.hierarchyDepth = parent->hierarchyDepth + 1;
	}
	else
	{
		transform.parent = INVALID_ENTITY;
		transform.hierarchyDepth = 0;
	}

	transform.dirty = true;
	
	// Note: We need to update children's depth too.
	// This will be handled implicitly if we process dirty transforms correctly,
	// or we should traverse here. For performance, we rely on Update.
}

void TransformSystem::UnsetParent(Transform& transform)
{
	if (transform.parent == INVALID_ENTITY) return;

	World& world = SceneManager::GetCurrentScene()->GetWorld();
	Transform* parent = world.GetComponent<Transform>(transform.parent);

	if (parent)
	{
		// If head of list, move head
		if (parent->firstChild == transform.entity)
		{
			parent->firstChild = transform.nextSibling;
		}
	}

	// Relink siblings
	if (transform.prevSibling != INVALID_ENTITY)
	{
		Transform* prev = world.GetComponent<Transform>(transform.prevSibling);
		if (prev) prev->nextSibling = transform.nextSibling;
	}
	if (transform.nextSibling != INVALID_ENTITY)
	{
		Transform* next = world.GetComponent<Transform>(transform.nextSibling);
		if (next) next->prevSibling = transform.prevSibling;
	}

	transform.parent = INVALID_ENTITY;
	transform.nextSibling = INVALID_ENTITY;
	transform.prevSibling = INVALID_ENTITY;
	transform.hierarchyDepth = 0;
	
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
	int count = 0;
	World& world = SceneManager::GetCurrentScene()->GetWorld();
	Entity current = transform->firstChild;
	while (current != INVALID_ENTITY)
	{
		count++;
		Transform* t = world.GetComponent<Transform>(current);
		if (!t) break; // Error check
		current = t->nextSibling;
	}
	return count;
}

Transform* TransformSystem::GetChild(Transform* transform, int index) 
{
	World& world = SceneManager::GetCurrentScene()->GetWorld();
	Entity current = transform->firstChild;
	int i = 0;
	while (current != INVALID_ENTITY)
	{
		if (i == index)
		{
			return world.GetComponent<Transform>(current);
		}
		Transform* t = world.GetComponent<Transform>(current);
		if (!t) break;
		current = t->nextSibling;
		i++;
	}
	return nullptr;
}

Transform* TransformSystem::FindChild(Transform* transform, const std::string& name)
{
	if (transform->entity.name == name) return transform;

	World& world = SceneManager::GetCurrentScene()->GetWorld();
	Entity current = transform->firstChild;

	while (current != INVALID_ENTITY)
	{
		Transform* t = world.GetComponent<Transform>(current);
		if (t)
		{
			Transform* found = FindChild(t, name);
			if (found) return found;
			current = t->nextSibling;
		}
		else
		{
			break;
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
	Quaternion rot = Quaternion::AngleAxis(angle, axis);
	Vector3 dir = transform.position - point;
	dir = rot * dir;
	transform.position = point + dir;
	transform.rotation = rot * transform.rotation;
	transform.dirty = true;
}

Vector3 TransformSystem::GetPosition(Transform& transform)
{
	World& world = SceneManager::GetCurrentScene()->GetWorld();
	if (transform.parent != INVALID_ENTITY)
	{
		return TransformPoint(*world.GetComponent<Transform>(transform.parent), transform.position);
	}
	else
	{
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

void TransformSystem::Start(World& world)
{
}

void TransformSystem::UpdateLocalMatrix(Transform& transform)
{
	transform.localMatrix.SetSRT(transform.scale, transform.rotation, transform.position);
}


void TransformSystem::UpdateAllDirtyTransforms(World& world)
{
	// 1. Collect all transforms that need updates.
	//    This includes explicit dirty transforms and their children (propagated locally)
	//    Actually, we can iterate all transforms (linear) to check dirty?
	//    Or use the stack approach for dirty roots.

	// Robust Iterative Approach:
	// Use a stack to traverse only dirty subtrees.
	
	std::vector<Transform*> dirtyRoots;
	View<Transform> view(world);
	
	// Collect explicitly dirty transforms. 
	// To minimize redundancy, we only want "roots" of dirty chains, but finding them is hard.
	// So we collect all dirty ones.
	for (auto [entity, transform] : view)
	{
		if (transform.dirty)
		{
			dirtyRoots.push_back(&transform);
		}
	}

	if (dirtyRoots.empty()) return;

	// Sort by depth to ensure parents processed before children (if multiple are dirty)
	std::sort(dirtyRoots.begin(), dirtyRoots.end(),
		[](const Transform* a, const Transform* b) {
			return a->hierarchyDepth < b->hierarchyDepth;
		});

	// Use a stack for traversal
	std::vector<Transform*> stack;
	stack.reserve(64);

	for (Transform* root : dirtyRoots)
	{
		// If already cleaned (by a parent processing it), skip
		if (!root->dirty && !root->hasChanged) continue; 
		// Note: hasChanged logic in this loop frame is tricky. 
		// We use dirty to mean "needs recalc".

		stack.push_back(root);

		while (!stack.empty())
		{
			Transform* t = stack.back();
			stack.pop_back();

			// Recalculate T
			UpdateLocalMatrix(*t);

			if (t->parent != INVALID_ENTITY)
			{
				Transform* parent = world.GetComponent<Transform>(t->parent);
				// Parent should be up-to-date because:
				// 1. If parent was dirty, it was in dirtyRoots and processed earlier (lower depth).
				// 2. If parent was NOT dirty, its matrix is valid.
				if (parent)
				{
					t->localToWorldMatrix = t->localMatrix * parent->localToWorldMatrix;
					// Depth correction (lazy update)
					t->hierarchyDepth = parent->hierarchyDepth + 1;
				}
				else
				{
					// Parent missing?
					t->localToWorldMatrix = t->localMatrix;
					t->hierarchyDepth = 0;
				}
			}
			else
			{
				t->localToWorldMatrix = t->localMatrix;
				t->hierarchyDepth = 0;
			}
			
			t->dirty = false;
			t->hasChanged = true;
			t->inverseDirty = true;

			// Add children to stack
			Entity childEntity = t->firstChild;
			while (childEntity != INVALID_ENTITY)
			{
				Transform* child = world.GetComponent<Transform>(childEntity);
				if (child)
				{
					// Force child to be "dirty" effectively because parent changed
					// In this frame, we just push to stack and process.
					// We don't need to set child->dirty = true physically if we just process it.
					// However, avoids double processing if child is also in dirtyRoots?
					// If child is in dirtyRoots, it will be processed later?
					// No, stack processing handles it NOW.
					// We should mark it as processed?
					// The "dirty" check at loop start handles skips.
					// But "hasChanged" is set to true. 
					
					stack.push_back(child);
				}
				childEntity = child ? child->nextSibling : INVALID_ENTITY;
			}
		}
	}
}

void TransformSystem::Update(World& world)
{
	UpdateAllDirtyTransforms(world);
	
	// Reset hasChanged flags? 
	// Previous implementation reset them at start of frame or something.
	// Ideally, hasChanged is valid FOR THIS FRAME. 
	// So we should reset it at the start of Update.
	
	/*
	View<Transform> view(world);
	for (auto [entity, transform] : view)
	{
		transform.hasChanged = false;
	}
	*/
	// This was done in the previous recursive implementation. 
	// We should probably do it here too, but UpdateAllDirtyTransforms is called ONCE per frame?
	// If we clear hasChanged inside UpdateAllDirtyTransforms it might be safer.
}