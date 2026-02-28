#include "EnemyAISystem.h"
#include "CharacterImporter.h"

void EnemyAISystem::Update(World& world)
{
    View<Transform, Enemy, AIAgent, ComboInput, ComboState, Animator> view(world);

    for (auto [entity, transform, enemy, aiAgent, comboInput, comboState, animator] : view)
    {
        if (!world.IsAlive(enemy.target)) continue;

        if (animator.currentClipName == "Hit_00" && animator.isPlaying)
        {
            continue;
        }

        Transform* targetTrans = world.GetComponent<Transform>(enemy.target);
        Vector3 toTarget = targetTrans->position - transform.position;
        float distSq = toTarget.SqrMagnitude();
        float dist = sqrtf(distSq);

        // 各種距離の設定
        const float attackRange = 160.0f;
        const float tooCloseRange = 110.0f; // これより近いと下がる

        // 1. 攻撃中（コンボ中）の処理
        if (comboState.currentMoveId != 0)
        {
            animator.isLoop = false;
            AIAgentSystem::GetInstance()->ResetAI(aiAgent); // 攻撃中は足を止める

            const auto& move = CharacterImporter::GetInstance()->GetMoveById(comboState.name, comboState.currentMoveId);
            float progress = comboState.timer / move.duration;

            // 追撃の判断
            if (progress >= move.inputStart && progress <= move.inputEnd && comboInput.attackInputType == AttackInputType::Idle)
            {
                // まだ射程内にいればAttack1を予約
                if (dist < attackRange + 30.0f) {
                    comboInput.attackInputType = AttackInputType::Attack1;
                }
            }
        }
        // 2. 非攻撃中（待機・移動・間合い管理）の処理
        else
        {
            comboInput.attackInputType = AttackInputType::Idle;

            // --- A. 攻撃後の離脱挙動 (クールダウン中) ---
            if (enemy.attackCoolDown > 0.0f)
            {
                enemy.attackCoolDown -= Time::GetDeltaTime();

                // ターゲットから離れる方向に目標地点を設定（ヒット＆アウェイ）
                Vector3 retreatDir = -toTarget.Normalized();
                Vector3 retreatPos = transform.position + retreatDir * 150.0f;
                AIAgentSystem::GetInstance()->SetDestination(aiAgent, retreatPos);
            }
            // --- B. 通常時の間合い管理 ---
            else
            {
                if (dist < tooCloseRange)
                {
                    // 近すぎる：少し後ろに下がる
                    Vector3 awayPos = transform.position - toTarget.Normalized() * 80.0f;
                    AIAgentSystem::GetInstance()->SetDestination(aiAgent, awayPos);
                }
                else if (dist < attackRange)
                {
                    // 射程内：攻撃開始
                    comboInput.attackInputType = AttackInputType::Attack1;

                    // 攻撃開始と同時にクールダウンをセット（これが攻撃後の離脱時間になる）
                    enemy.attackCoolDown = 2.0f;
                    AIAgentSystem::GetInstance()->ResetAI(aiAgent);
                }
                else
                {
                    // 遠い：どこまでも追いかける（距離制限を撤廃）
                    AIAgentSystem::GetInstance()->SetDestination(aiAgent, targetTrans->position);
                }
            }
        }
    }
}