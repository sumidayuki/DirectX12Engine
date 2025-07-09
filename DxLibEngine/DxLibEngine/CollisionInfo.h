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
	Entity other = NullEntity;
	CollisionState state = CollisionState::None;
};