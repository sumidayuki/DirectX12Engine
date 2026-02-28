#pragma once

struct WorldTransform : public IComponentData
{
	Matrix4x4 localToWorldMatrix = Matrix4x4::identity;
	Matrix4x4 worldToLocalMatrix = Matrix4x4::identity;
};