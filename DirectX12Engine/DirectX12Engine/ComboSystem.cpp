#include "Precompiled.h"
#include "ComboSystem.h"
#include "ComboState.h"
#include "ComboInput.h"
#include "ComboMove.h"
#include "CharacterStatus.h"
#include "CharacterImporter.h"

void ComboSystem::TransionTo(ComboState& state, uint32_t newMoveID)
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
    input.inputKey = InputKey::None;
    input.timer = 0.0f; // InputSystem側のバッファ時間をリセット
}

uint32_t ComboSystem::GetNextMoveID(const std::string& name, InputKey input, const std::vector<uint32_t>& possibles)
{
    for (uint32_t id : possibles)
    {
        // 遷移先候補のMoveデータを取得
        const ComboMove& move = CharacterImporter::GetInstance()->GetMoveById(name, id);

        // プレイヤーの入力(type)が、その技の必要入力(requiredInput)と一致するか判定
        if (move.inputKey == input)
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
            if (input.inputKey != InputKey::None)
            {
                for (const auto& move : charInfo->moves)
                {
                    if (move.isStarter && move.inputKey == input.inputKey)
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

		// 攻撃しないムーブの場合は、コンボ入力の判定のみ行う
        if (!currentMove.isAttack)
        {
            // コンボ入力の判定
            if (input.inputKey != InputKey::None)
            {
                uint32_t nextID = GetNextMoveID(state.name, input.inputKey, currentMove.nextPossibleMoves);
                if (nextID != -1)
                {
                    TransionTo(state, nextID);
                    ClearInput(input);
                }
            }
            else
            {
                ResetCombo(state);
                ClearInput(input);
            }

            continue;
        }

        AITrigger* trigger = world.GetComponent<AITrigger>(entity);
        bool canInput = (progress >= currentMove.inputStart && progress <= currentMove.inputEnd);

        if (trigger && canInput)
        {
            trigger->triggers.push_back("OnComboWindow"_h);
        }

        // コンボ入力の判定
        if (input.inputKey != InputKey::None)
        {
            if (canInput)
            {
                uint32_t nextID = GetNextMoveID(state.name, input.inputKey, currentMove.nextPossibleMoves);
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
            if (trigger)
            {
				trigger->triggers.push_back("OnComboFinished"_h);
            }

            ResetCombo(state);
            ClearInput(input);
        }
    }
}