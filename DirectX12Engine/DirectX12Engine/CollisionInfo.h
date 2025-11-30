#pragma once

enum class CollisionState
{
	None,
	Enter,
	Stay,
	Exit,
};

struct CollisionInfo : IComponentData
{
	Entity other = INVALID_ENTITY;
	CollisionState state = CollisionState::None;
};