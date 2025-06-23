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
	Entity other = (Entity) - 1;
	CollisionState state = CollisionState::None;
};