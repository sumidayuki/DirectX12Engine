#pragma once

enum class MoveType
{
    Attack,
    Guard,
    Rolling
};

struct AttackParams
{
    float damage;
    float inputStart;
    float inputEnd;
    float hitStartTime;
    float hitEndTime;
    bool allowCancel;
    std::vector<std::string> colliderNames;
};

struct GuardParams
{
};

struct RollingParams
{
    float moveSpeed;                            // 移動速度
    float invincibleStart;                      // 無敵開始時間（0~1正規化）
    float invincibleEnd;                        // 無敵終了時間（0~1正規化）
};

struct MoveData
{
	uint32_t moveId;                                                    // ユニークID
	std::string textName;                                               // ガイドのテキストファイル名
	InputKey inputKey;                                                  // 必要な入力キー
	MoveType type;                                                      // ムーブの種類

	bool isStarter;                                                     // 始動技判定フラグ
	float duration;                                                     // ムーブ総時間
	float staminaCost;                                                  // スタミナ消費量

	std::variant<AttackParams, GuardParams, RollingParams> params;      // ムーブの種類に応じたパラメータ

	std::vector<uint32_t> nextPossibleMoves;                            // 次ムーブIDリスト
	std::string animationName;                                          // アニメーション名
};