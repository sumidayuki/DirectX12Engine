#include "EnemyAISystem.h"
#include "CharacterImporter.h"

void EnemyAISystem::Update(World& world)
{
    View<Transform, Enemy, AIAgent, ComboInput, ComboState, Animator> view(world);

    for (auto [entity, transform, enemy, aiAgent, comboInput, comboState, animator] : view)
    {
        if (!world.IsAlive(enemy.target)) continue;

        // 被弾中は思考停止
        if (animator.currentClipName == "Hit_00" && animator.isPlaying) continue;

        Transform* targetTrans = world.GetComponent<Transform>(enemy.target);
        Vector3 toTarget = targetTrans->position - transform.position;
        float dist = toTarget.Magnitude();

        // 距離設定
        const float jumpAttackMinRange = 500.0f; // ジャンプ攻撃が届く最小距離
		const float jumpAttackMaxRange = 800.0f;    // ジャンプ攻撃が届く最大距離
        const float attackRange = 160.0f;     // 通常攻撃の距離
        const float tooCloseRange = 100.0f;

        // 攻撃中（コンボ中）の処理
        if (comboState.currentMoveId != 0)
        {
            animator.isLoop = false;
            AIAgentSystem::GetInstance()->ResetAI(aiAgent);

            const auto& move = CharacterImporter::GetInstance()->GetMoveById(comboState.name, comboState.currentMoveId);
            float progress = comboState.timer / move.duration;

            // 追撃判定 (Input受付時間内かつ、まだ次を入力していない場合)
            if (progress >= move.inputStart && progress <= move.inputEnd && comboInput.attackInputType == AttackInputType::Idle)
            {
                if (tooCloseRange > dist)
				{
					// 近すぎる場合は一度距離を取る
					Vector3 awayPos = transform.position - toTarget.Normalized() * (tooCloseRange - dist);
					AIAgentSystem::GetInstance()->SetDestination(aiAgent, awayPos);
				}
				else if (dist >= tooCloseRange && dist < attackRange)
				{
					// 通常攻撃の範囲内なら、次の攻撃（Attack2）を繰り出す
					comboInput.attackInputType = AttackInputType::Attack1;
				}
            }
        }
        // 非攻撃中の処理
        else
        {
            comboInput.attackInputType = AttackInputType::Idle;

            // クールダウン中の離脱挙動
            if (enemy.attackCoolDown > 0.0f)
            {
                enemy.attackCoolDown -= Time::GetDeltaTime();
                Vector3 retreatDir = -toTarget.Normalized();
                Vector3 retreatPos = transform.position + retreatDir * 150.0f;
                AIAgentSystem::GetInstance()->SetDestination(aiAgent, retreatPos);
            }
            else
            {
                // 攻撃選択ロジック

				// ジャンプ攻撃の射程外
                if (dist > jumpAttackMinRange && dist < jumpAttackMaxRange)
                {
                    // ジャンプ攻撃（Attack3）を繰り出す
                    comboInput.attackInputType = AttackInputType::Attack3;
                    enemy.attackCoolDown = 3.5f; // ジャンプ後は隙が大きいので長めのクールダウン
                    AIAgentSystem::GetInstance()->ResetAI(aiAgent);
                }
                // 通常攻撃の射程内
                else if (dist < attackRange)
                {
                    if (dist < tooCloseRange) 
                    {
                        // 近すぎる場合は仕切り直し
                        Vector3 awayPos = transform.position - toTarget.Normalized() * 80.0f;
                        AIAgentSystem::GetInstance()->SetDestination(aiAgent, awayPos);
                    }
                    else 
                    {
                        // 通常攻撃開始
                        comboInput.attackInputType = AttackInputType::Attack1;
                        enemy.attackCoolDown = 2.0f;
                        AIAgentSystem::GetInstance()->ResetAI(aiAgent);
                    }
                }
                // 索敵・追跡
                else
                {
                    AIAgentSystem::GetInstance()->SetDestination(aiAgent, targetTrans->position);
                }
            }
        }
    }
}