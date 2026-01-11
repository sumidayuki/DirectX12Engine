#pragma once

struct ComboMove : IComponentData
{
    int moveId;                             // ユニークID
    AttackInputType requiredInput;          // この技を出すために必要な入力タイプ
    bool isStarter;                         // 始動技判定フラグ
    float duration;                         // ムーブ総時間
    float inputStart;                       // 入力開始時間（0~1正規化）
    float inputEnd;                         // 入力終了時間（0~1正規化）
    bool allowCancel;                       // キャンセル可
    std::vector<int> nextPossibleMoves;     // 次ムーブIDリスト
    int damage;
    std::string animationName;
};