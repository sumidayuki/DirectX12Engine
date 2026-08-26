#include "TransformSystem.h"
#include <algorithm>
#include <vector>
#include <vector>
#include <stack>
#include "World.h"
#include "SceneManager.h"


void TransformSystem::Start(World& world)
{
}

void TransformSystem::UpdateAllDirtyTransforms(World& world)
{
	std::vector<Transform*> dirtyRoots;
	View<Transform> view(world);

	for (auto [entity, transform] : view)
	{
		if (transform.dirty)
		{
			dirtyRoots.push_back(&transform);
		}
	}

	if (dirtyRoots.empty()) return;

	std::sort(dirtyRoots.begin(), dirtyRoots.end(),
		[](const Transform* a, const Transform* b) {
			return a->hierarchyDepth < b->hierarchyDepth;
		});

	std::vector<Transform*> stack;
	stack.reserve(64);

	for (Transform* root : dirtyRoots)
	{
		if (!root->dirty && !root->hasChanged) continue;

		stack.push_back(root);

		while (!stack.empty())
		{
			Transform* t = stack.back();
			stack.pop_back();

			TransformAPI::UpdateLocalMatrix(*t);

			if (t->parent != INVALID_ENTITY)
			{
				Transform* parent = world.GetComponent<Transform>(t->parent);

				if (parent)
				{
					t->localToWorldMatrix = t->localMatrix * parent->localToWorldMatrix;
					t->hierarchyDepth = parent->hierarchyDepth + 1;
				}
				else
				{
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

			Entity childEntity = t->firstChild;
			while (childEntity != INVALID_ENTITY)
			{
				Transform* child = world.GetComponent<Transform>(childEntity);
				if (child)
				{
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
}