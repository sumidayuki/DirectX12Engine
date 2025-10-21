#pragma once

enum class CollisionState
{
	None,
	Enter,
	Stay,
	Exit,
};

struct CollisionInfo
{
	Entity other = INVALID_ENTITY;
	CollisionState state = CollisionState::None;
};