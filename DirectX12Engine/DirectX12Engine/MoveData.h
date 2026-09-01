#pragma once

#include "InputBind.h"

// Moveの種類
enum class MoveType
{
	Idle,
	Attack,
	Guard,
	Rolling
};

// Moveの遷移条件
enum class MoveTransitionType
{
	InputPressed,
	InputReleased,
	MoveEnd
};

// Move間の遷移情報
struct MoveTransition
{
	uint32_t targetMoveId = 0;

	MoveTransitionType type = MoveTransitionType::InputPressed;

	InputKey inputKey = InputKey::None;

	// 現在Moveの進行度(0～1)に対する遷移可能範囲
	float start = 0.0f;
	float end = 1.0f;
};

// Idle
struct IdleParams
{
};

// Attack
struct AttackParams
{
	std::string animationName;

	float damage = 0.0f;

	float hitStartTime = 0.0f;
	float hitEndTime = 0.0f;

	std::vector<std::string> colliderNames;
};

// Guard
struct GuardParams
{
};

// Rolling
struct RollingParams
{
	std::string animationName;

	float moveSpeed = 0.0f;

	float invincibleStart = 0.0f;
	float invincibleEnd = 0.0f;
};

// Moveデータ
struct MoveData
{
	uint32_t moveId = 0;

	std::string textName;

	MoveType type = MoveType::Idle;

	float duration = 0.0f;
	float staminaCost = 0.0f;

	std::variant<
		IdleParams,
		AttackParams,
		GuardParams,
		RollingParams
	> params;

	std::vector<MoveTransition> transitions;
};