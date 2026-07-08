#pragma once

struct ComboMove
{
    uint32_t moveId;                            // ユニークID
	std::string textName;                       // ガイドのテキストファイル名
    InputKey inputKey;                          // 入力キー
    bool isStarter;                             // 始動技判定フラグ
    bool isAttack;
    float duration;                             // ムーブ総時間
    float inputStart;                           // 入力開始時間（0~1正規化）
    float inputEnd;                             // 入力終了時間（0~1正規化）
    float hitStartTime;                         // ヒット判定開始時間（0~1正規化）
    float hitEndTime;                           // ヒット判定終了時間（0~1正規化）
    bool allowCancel;                           // キャンセル可
    std::vector<uint32_t> nextPossibleMoves;    // 次ムーブIDリスト
    float damage;
    std::string animationName;
};