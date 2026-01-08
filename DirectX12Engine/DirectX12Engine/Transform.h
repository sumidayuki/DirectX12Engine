#pragma once

struct Transform : public IComponentData
{
	Entity entity;
	Entity parent = INVALID_ENTITY;
	// Sibling Linked List for hierarchy
	Entity firstChild = INVALID_ENTITY;
	Entity nextSibling = INVALID_ENTITY;
	Entity prevSibling = INVALID_ENTITY;
	
	bool isActive = true;
	
	Vector3 position = Vector3(0.0f, 0.0f, 0.0f);
	Quaternion rotation = Quaternion::identity;
	Vector3 scale = Vector3(1.0f, 1.0f, 1.0f);
	
	Matrix4x4 localMatrix = Matrix4x4::identity;
	Matrix4x4 localToWorldMatrix = Matrix4x4::identity;
	Matrix4x4 worldToLocalMatrix = Matrix4x4::identity;
	
	bool dirty = true;
	bool hasChanged = true;
	bool inverseDirty = true;
	
	uint16_t hierarchyDepth = 0;
};
