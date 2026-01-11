#include "Precompiled.h"
#include "ComboSystem.h"
#include "ComboState.h"
#include "ComboInput.h"
#include "ComboMove.h"

const ComboMove& ComboSystem::GetMoveById(const std::string& name, int id) const
{
    if (m_moves.contains(name))
    {
        for (auto& move : m_moves.at(name))
        {
            if (move.moveId == id)
            {
                return move;
            }
        }
    }
    assert(false && "MoveID or Name not found");
    static ComboMove empty; return empty;
}

void ComboSystem::TransionTo(ComboState& state, int newMoveID)
{
    state.currentMoveID = newMoveID;
    state.comboIndex++;
    state.timer = 0.0f; // 次の技の経過時間をリセット
}

void ComboSystem::ResetCombo(ComboState& state)
{
    state.currentMoveID = 0;
    state.comboIndex = 0;
    state.timer = 0.0f;
}

void ComboSystem::ClearInput(ComboInput& input)
{
    input.attackInputType = AttackInputType::Idle;
    input.timer = 0.0f; // InputSystem側のバッファ時間をリセット
}

int ComboSystem::GetNextMoveID(const std::string& name, AttackInputType type, const std::vector<int>& possibles)
{
    for (int id : possibles)
    {
        // 遷移先候補のMoveデータを取得
        const ComboMove& move = GetMoveById(name, id);

        // プレイヤーの入力(type)が、その技の必要入力(requiredInput)と一致するか判定
        if (move.requiredInput == type)
        {
            return id; // 条件に一致する技のIDを返す
        }
    }

    return -1; // 一致する派生先がない場合
}

bool ComboSystem::Load(World& world)
{
    // 1. JSONファイルを開く
    std::ifstream file("Assets/Json/Combos.json");
    if (!file.is_open())
    {
        // ファイルが開けない場合のエラー処理
        return false;
    }

    Json data;
    try
    {
        // 2. JSONをパース
        file >> data;

        // 3. 各キャラクター（"Player", "Enemy_Soldier"等）ごとにループ
        for (auto& [characterName, moveList] : data.items())
        {
            std::vector<ComboMove> moves;

            // 4. キャラクターに紐づくムーブの配列を処理
            for (auto& m : moveList)
            {
                ComboMove move;
                move.moveId = m["moveId"];                     // ユニークID
                move.duration = m["duration"];               // ムーブ総時間
                move.inputStart = m["inputStart"];           // 入力開始時間
                move.inputEnd = m["inputEnd"];               // 入力終了時間
                move.allowCancel = m["allowCancel"];         // キャンセル可否
                move.damage = m["damage"];                   // ダメージ量
                move.animationName = m["animationName"];     // アニメーション名

                // std::vector<int> の読み込み
                move.nextPossibleMoves = m["nextPossibleMoves"].get<std::vector<int>>();

                moves.push_back(move);
            }

            // 5. システム内部のマップに登録
            m_moves[characterName] = moves;
        }
    }
    catch (const Json::exception& e)
    {
        // JSONの構造が間違っている場合のエラーハンドリング
        // printf("JSON Parse Error: %s\n", e.what());
        return false;
    }

    return true;
}

void ComboSystem::Update(World& world)
{
    View<ComboState, ComboInput> view(world);

    for (auto [entity, state, input] : view)
    {
        // 状態の更新
        state.timer += Time::GetDeltaTime();

        // 現在のムーブ情報の取得
        // currentMoveID が 0 の場合は待機状態
        if (state.currentMoveID == 0)
        {
            if (input.attackInputType != AttackInputType::Idle)
            {
                // 全ムーブの中から、isStarter が true かつ 入力が一致するものを探す
                for (const auto& move : m_moves[state.name])
                {
                    if (move.isStarter && move.requiredInput == input.attackInputType)
                    {
                        TransionTo(state, move.moveId);
                        ClearInput(input);
                        break;
                    }
                }
            }
            continue;
        }

        const ComboMove& currentMove = GetMoveById(state.name, state.currentMoveID);

        // アニメーション進捗の計算 (0.0 ~ 1.0)
        float progress = state.timer / currentMove.duration;

        // コンボ入力の判定
        if (input.attackInputType != AttackInputType::Idle)
        {
            // 入力受付期間内か
            if (progress >= currentMove.inputStart && progress <= currentMove.inputEnd)
            {
                // nextPossibleMoves の中から、入力が一致する派生技を探す
                int nextID = GetNextMoveID(state.name, input.attackInputType, currentMove.nextPossibleMoves);
                if (nextID != -1)
                {
                    TransionTo(state, nextID);
                    ClearInput(input);
                    continue;
                }
            }
        }

        // 技の終了判定
        if (progress >= 1.0f)
        {
            ResetCombo(state);
        }
    }
}