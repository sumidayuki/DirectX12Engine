#include "Precompiled.h"
#include "MoveSystem.h"
#include "MoveState.h"
#include "MoveInput.h"
#include "CharacterStatus.h"
#include "CharacterImporter.h"

void MoveSystem::TransitionTo(MoveState& state, uint32_t newMoveID)
{
    state.currentMoveId = newMoveID;
    state.comboIndex++;
    state.hitConfirm = false;
    state.canHit = false;
    state.isAnimed = false;
    state.timer = 0.0f; // 次の技の経過時間をリセット
}

void MoveSystem::ResetMove(MoveState& state)
{
    state.currentMoveId = 0;
    state.comboIndex = 0;
    state.canHit = false;
    state.hitConfirm = false;
    state.isAnimed = false;
    state.timer = 0.0f;
}

void MoveSystem::ClearInput(MoveInput& input)
{
    input.inputKey = InputKey::None;
    input.timer = 0.0f;
}

uint32_t MoveSystem::GetNextMoveID(const std::string& name, InputKey input, const std::vector<uint32_t>& possibles)
{
    for (uint32_t id : possibles)
    {
        // 遷移先候補のMoveデータを取得
        const MoveData& move = CharacterInfoRegistry::GetInstance()->GetMoveById(name, id);

        // プレイヤーの入力(type)が、その技の必要入力(requiredInput)と一致するか判定
        if (move.inputKey == input)
        {
            return id; // 条件に一致する技のIDを返す
        }
    }

    return -1; // 一致する派生先がない場合
}

void MoveSystem::Update(World& world)
{
    View<MoveState, MoveInput> view(world);

    for (auto [entity, state, input] : view)
    {
        const auto* charInfo = CharacterInfoRegistry::GetInstance()->GetCharacterInfo(state.name);
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
                        TransitionTo(state, move.moveId);
                        ClearInput(input);
                        break;
                    }
                }
            }
            continue;
        }

        const MoveData& currentMove = CharacterInfoRegistry::GetInstance()->GetMoveById(state.name, state.currentMoveId);
        float progress = (currentMove.duration > 0.0f) ? state.timer / currentMove.duration : 1.0f;

        AITrigger* trigger = world.GetComponent<AITrigger>(entity);

        switch (currentMove.type)
        {
            // ガード
            case MoveType::Guard:
            {
                // ガード中に別の入力があれば派生を試みる
                if (input.inputKey != InputKey::None && input.inputKey != InputKey::Guard)
                {
                    uint32_t nextID = GetNextMoveID(state.name, input.inputKey, currentMove.nextPossibleMoves);
                    if (nextID != -1)
                    {
                        TransitionTo(state, nextID);
                        ClearInput(input);
                        break;
                    }
                }

                // ガード入力がなくなったらリセット
                if (input.inputKey != InputKey::Guard)
                {
                    ResetMove(state);
                    ClearInput(input);
                }
                break;
            }

            // ローリング
            case MoveType::Rolling:
            {
                if (progress >= 1.0f)
                {
                    ResetMove(state);
                    ClearInput(input);
                }
                break;
            }

            // 攻撃
            case MoveType::Attack:
            {
                const AttackParams& params = std::get<AttackParams>(currentMove.params);

                bool canInput = (progress >= params.inputStart && progress <= params.inputEnd);

                // 攻撃が開始したことを発信
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
                            TransitionTo(state, nextID);
                            ClearInput(input);
                            break;
                        }
                    }
                }

                if (!state.canHit)
                {
                    // ヒット開始時間になったら、攻撃開始処理を行う
                    if (progress >= params.hitStartTime)
                    {
                        Attackable* attackable = world.GetComponent<Attackable>(entity);
                        attackable->entities.clear();
                        attackable->isAttacking = true;
                        state.canHit = true;

                        if (params.colliderNames.size() > 0)
                        {
                            CharacterHitboxes* hitboxes = world.GetComponent<CharacterHitboxes>(entity);
                            for (auto& colliderName : params.colliderNames)
                            {
                                auto it = hitboxes->entities.find(HashString32(colliderName.c_str()));
                                if (it != hitboxes->entities.end())
                                {
                                    Entity colliderEntity = it->second;
                                    Collider* collider = world.GetComponent<Collider>(colliderEntity);
                                    if (collider)
                                    {
                                        collider->isEnable = true;
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    Attackable* attackable = world.GetComponent<Attackable>(entity);

                    // ヒット終了時間前なら、衝突判定とダメージ処理を行う
                    if (progress < params.hitEndTime)
                    {
                        if (params.colliderNames.size() > 0)
                        {
                            CharacterHitboxes* hitboxes = world.GetComponent<CharacterHitboxes>(entity);
                            for (auto& colliderName : params.colliderNames)
                            {
                                auto it = hitboxes->entities.find(HashString32(colliderName.c_str()));
                                if (it != hitboxes->entities.end())
                                {
                                    Entity colliderEntity = it->second;
                                    Collider* collider = world.GetComponent<Collider>(colliderEntity);
                                    if (collider->info.state == CollisionState::Enter || collider->info.state == CollisionState::Stay)
                                    {
                                        Entity target = collider->info.other;
                                        auto it = std::find(attackable->entities.begin(), attackable->entities.end(), target);
                                        if (it == attackable->entities.end())
                                        {
                                            Damageable* damageable = world.GetComponent<Damageable>(target);
                                            if (damageable)
                                            {
                                                Damage damage;
                                                damage.type = DamageType::Normal;
                                                damage.damage = params.damage;
                                                damageable->damageQueue.push(damage);
                                                attackable->entities.push_back(target);
                                            }
                                            state.hitConfirm = true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    // ヒット終了時間を過ぎたら、攻撃終了処理を行う
                    else
                    {
                        attackable->isAttacking = false;

                        if (params.colliderNames.size() > 0)
                        {
                            CharacterHitboxes* hitboxes = world.GetComponent<CharacterHitboxes>(entity);
                            for (auto& colliderName : params.colliderNames)
                            {
                                auto it = hitboxes->entities.find(HashString32(colliderName.c_str()));
                                if (it != hitboxes->entities.end())
                                {
                                    Entity colliderEntity = it->second;
                                    Collider* collider = world.GetComponent<Collider>(colliderEntity);
                                    if (collider && collider->isEnable)
                                    {
                                        collider->isEnable = false;
                                    }
                                }
                            }
                        }
                    }
                }

                if (progress >= 1.0f)
                {
                    if (trigger)
                    {
                        trigger->triggers.push_back("OnComboFinished"_h);
                    }

                    ResetMove(state);
                    ClearInput(input);
                }
                break;
            }
        }
    }
}

