#pragma once

struct LocalTransform : public IComponentData
{
	Vector3 position = Vector3::zero;
	Quaternion rotation = Quaternion::identity;
	Vector3 scale = Vector3::one;
};