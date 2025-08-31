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
	Entity* other = nullptr;
	CollisionState state = CollisionState::None;
};