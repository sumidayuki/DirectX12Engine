#include "Precompiled.h"
#include "ComboSystem.h"
#include "ComboState.h"
#include "ComboInput.h"
#include "ComboMove.h"
#include "CharacterStatus.h"
#include "CharacterImporter.h"

void ComboSystem::TransionTo(ComboState& state, int newMoveID)
{
    state.currentMoveId = newMoveID;
    state.comboIndex++;
    state.hitConfirm = false;
	state.canHit = false;
	state.isAnimed = false;
    state.timer = 0.0f; // 次の技の経過時間をリセット
}

void ComboSystem::ResetCombo(ComboState& state)
{
    state.currentMoveId = 0;
    state.comboIndex = 0;
	state.canHit = false;
	state.hitConfirm = false;
	state.isAnimed = false;
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
        const ComboMove& move = CharacterImporter::GetInstance()->GetMoveById(name, id);

        // プレイヤーの入力(type)が、その技の必要入力(requiredInput)と一致するか判定
        if (move.requiredInput == type)
        {
            return id; // 条件に一致する技のIDを返す
        }
    }

    return -1; // 一致する派生先がない場合
}

void ComboSystem::Update(World& world)
{
    View<ComboState, ComboInput> view(world);

    for (auto [entity, state, input] : view)
    {
        const auto* charInfo = CharacterImporter::GetInstance()->GetCharacterInfo(state.name);
        if (!charInfo) continue;

        state.timer += Time::GetDeltaTime();

        if (state.currentMoveId == 0)
        {
            if (input.attackInputType != AttackInputType::Idle)
            {
                for (const auto& move : charInfo->moves)
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

        const ComboMove& currentMove = CharacterImporter::GetInstance()->GetMoveById(state.name, state.currentMoveId);
        float progress = state.timer / currentMove.duration;

        // コンボ入力の判定
        if (input.attackInputType != AttackInputType::Idle)
        {
            if (progress >= currentMove.inputStart && progress <= currentMove.inputEnd)
            {
                int nextID = GetNextMoveID(state.name, input.attackInputType, currentMove.nextPossibleMoves);
                if (nextID != -1)
                {
                    TransionTo(state, nextID);
                    ClearInput(input);
                    continue;
                }
            }
        }

        // ヒット判定の更新
        if (!state.canHit)
        {
            if (progress >= currentMove.hitStartTime)
            {
                state.canHit = true;
            }
        }
        else
        {
            if (progress >= currentMove.hitEndTime)
            {
                state.canHit = false;
            }
        }

        if (progress >= 1.0f)
        {
            ResetCombo(state);
            ClearInput(input);
        }
    }
}