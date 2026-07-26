#pragma once
#include "InputBind.h"

enum class MoveType
{
    Attack,
    Guard,
    Rolling
};

struct AttackParams
{
    std::string animationName;
    float damage = 0.0f;
    float inputStart = 0.0f;
    float inputEnd = 0.0f;
    float hitStartTime = 0.0f;
    float hitEndTime = 0.0f;
    std::vector<std::string> colliderNames;
};

struct GuardParams
{
};

struct RollingParams
{
    std::string animationName;
    float moveSpeed;                            // 移動速度
    float invincibleStart;                      // 無敵開始時間（0~1正規化）
    float invincibleEnd;                        // 無敵終了時間（0~1正規化）
};

struct MoveData
{
	uint32_t moveId = 0;                                                // ユニークID
	std::string textName;                                               // ガイドのテキストファイル名
	InputKey inputKey = InputKey::None;                                 // 必要な入力キー
	MoveType type = MoveType::Attack;                                   // ムーブの種類

    float duration = 0.0f;
	bool isStarter = false;                                             // 始動技判定フラグ
	float staminaCost = 0.0f;                                           // スタミナ消費量

	std::variant<AttackParams, GuardParams, RollingParams> params;      // ムーブの種類に応じたパラメータ

	std::vector<uint32_t> nextPossibleMoves;                            // 次ムーブIDリスト
};