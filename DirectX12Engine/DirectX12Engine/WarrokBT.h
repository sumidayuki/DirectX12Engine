#pragma once
#include "CharacterImporter.h"

namespace WarrokBT
{
	inline std::unique_ptr<BTNode> InRange(float range)
	{
		return std::make_unique<BTCondition>([range](Blackboard& bb) 
		{
			return bb.GetFloat("distToTarget") < range;
		});
	}

	inline std::unique_ptr<BTNode> Create()
	{
		auto root = std::make_unique<BTSelector>();

		// ターゲットが死んでいたら何もしない
		{
			auto seq = std::make_unique<BTSequence>();

			auto aliveCond = std::make_unique<BTCondition>([](Blackboard& bb)
			{
				return bb.world->IsAlive(bb.target);
			});
			seq->AddChild(std::make_unique<BTInverter>(std::move(aliveCond)));

			seq->AddChild(std::make_unique<BTAction>([](Blackboard& bb) 
			{
				AIAgent* ai = bb.world->GetComponent<AIAgent>(bb.self);
				if (ai) AIAgentSystem::GetInstance()->ResetAI(*ai);
				return BTStatus::Success;
			}));

			root->AddChild(std::move(seq));
		}

		// 被弾中は何もしない
		{
			auto seq = std::make_unique<BTSequence>();
			seq->AddChild(std::make_unique<BTCondition>([](Blackboard& bb) 
			{
				Animator* animator = bb.world->GetComponent<Animator>(bb.self);
				return animator && animator->currentClipName == "Hit_00" && animator->isPlaying;
			}));
			seq->AddChild(std::make_unique<BTAction>([](Blackboard& bb) 
			{
				return BTStatus::Success;
			}));
			root->AddChild(std::move(seq));
		}

		// 硬直中は何もしない
		{
			auto seq = std::make_unique<BTSequence>();
			seq->AddChild(std::make_unique<BTCondition>([](Blackboard& bb) 
			{
				return bb.GetBool("IsRecovery");
			}));
			seq->AddChild(std::make_unique<BTAction>([](Blackboard& bb) 
			{
				AIAgent* ai = bb.world->GetComponent<AIAgent>(bb.self);
				if (ai) AIAgentSystem::GetInstance()->ResetAI(*ai);
				return BTStatus::Success;
			}));
			root->AddChild(std::move(seq));
		}

		// 3連続ジャンプアタック
		{
			auto seq = std::make_unique<BTSequence>();

			seq->AddChild(std::make_unique<BTCondition>([](Blackboard& bb) 
			{
				if (bb.GetBool("JumpAttackReady")) 
				{
					bb.SetBool("JumpAttackStarted", false);
					return true;
				}
				return false;
			}));

			auto jumpAttackAction = std::make_unique<BTAction>([](Blackboard& bb) 
			{
				ComboState* state = bb.world->GetComponent<ComboState>(bb.self);
				ComboInput* input = bb.world->GetComponent<ComboInput>(bb.self);
				if (!state || !input) return BTStatus::Failure;

				// 既に攻撃が始動している場合
				if (bb.GetBool("JumpAttackStarted"))
				{
					// 攻撃が終了したか、何らかの理由で中断された場合
					if (state->currentMoveId != 3)
					{
						bb.SetBool("JumpAttackStarted", false);
						return BTStatus::Success;
					}

					const auto& move = CharacterImporter::GetInstance()->GetMoveById(state->name, state->currentMoveId);
					float progress = state->timer / move.duration;

					// 次のコンボ入力受付期間に入ったら Success を返し、リピーターに次の入力をさせる
					if (progress >= move.inputStart && progress <= move.inputEnd)
					{
						bb.SetBool("JumpAttackStarted", false);
						return BTStatus::Success;
					}

					return BTStatus::Running;
				}

				// まだ攻撃が始動していない（または次のコンボの初期待ち）
				if (state->currentMoveId == 3)
				{
					const auto& move = CharacterImporter::GetInstance()->GetMoveById(state->name, state->currentMoveId);
					float progress = state->timer / move.duration;
					// アニメーションが始まったばかりの時のみ Started を true にする
					if (progress < move.inputStart)
					{
						bb.SetBool("JumpAttackStarted", true);
						return BTStatus::Running;
					}
				}

				// 攻撃のトリガーを入力
				input->attackInputType = AttackInputType::Attack3;
				return BTStatus::Running;
			});

			seq->AddChild(std::make_unique<BTRepeater>(std::move(jumpAttackAction), 3));

			// 3回終わった後にクールダウン
			seq->AddChild(std::make_unique<BTAction>([](Blackboard& bb) 
			{
				Enemy* enemy = bb.world->GetComponent<Enemy>(bb.self);
				if (enemy) 
				{
					enemy->jumpAttackCoolDownTimer = enemy->jumpAttackCoolDown;
				}
				return BTStatus::Success;
			}));

			root->AddChild(std::move(seq));
		}

		// 通常の攻撃 (接近ロジック含む)
		{
			auto attackProcess = std::make_unique<BTSequence>();

			attackProcess->AddChild(std::make_unique<BTCondition>([](Blackboard& bb) 
			{
				return bb.GetBool("PunchAttackReady");
			}));

			attackProcess->AddChild(std::make_unique<BTAction>([](Blackboard& bb) 
			{
				AIAgent* aiAgent = bb.world->GetComponent<AIAgent>(bb.self);
				if (aiAgent) AIAgentSystem::GetInstance()->ResetAI(*aiAgent);
				return BTStatus::Success;
			}));

			auto positionSelector = std::make_unique<BTSelector>();
			auto moveBranch = std::make_unique<BTSequence>();

			moveBranch->AddChild(std::make_unique<BTInverter>(InRange(160.0f)));
			moveBranch->AddChild(std::make_unique<BTAction>([](Blackboard& bb) 
			{
				auto* agent = bb.world->GetComponent<AIAgent>(bb.self);
				auto* targetTrans = bb.world->GetComponent<Transform>(bb.target);
				if (!agent || !targetTrans) return BTStatus::Failure;

				agent->speed = 300;
				AIAgentSystem::GetInstance()->SetDestination(*agent, targetTrans->position);
				float dist = bb.GetFloat("distToTarget");
				return (dist > 160.0f) ? BTStatus::Running : BTStatus::Success;
			}));
			positionSelector->AddChild(std::move(moveBranch));
			positionSelector->AddChild(std::make_unique<BTAction>([](Blackboard& bb) { return BTStatus::Success; }));

			attackProcess->AddChild(std::move(positionSelector));

			attackProcess->AddChild(std::make_unique<BTAction>([](Blackboard& bb) 
			{
				ComboState* state = bb.world->GetComponent<ComboState>(bb.self);
				ComboInput* input = bb.world->GetComponent<ComboInput>(bb.self);
				if (!state || !input) return BTStatus::Failure;

				// 攻撃アニメーション再生中
				if (state->currentMoveId != 0)
				{
					Animator* animator = bb.world->GetComponent<Animator>(bb.self);
					if (animator) animator->isLoop = false;

					bb.SetBool("PunchStarted", true);

					const auto& move = CharacterImporter::GetInstance()->GetMoveById(state->name, state->currentMoveId);
					float progress = state->timer / move.duration;

					// 入力受付窓中にコンボ入力でつなげる
					if (progress >= move.inputStart && progress <= move.inputEnd
						&& input->attackInputType == AttackInputType::Idle)
					{
						input->attackInputType = AttackInputType::Attack1;
					}
					return BTStatus::Running;
				}

				// 攻撃が終了した（moveIdが0に戻った）→ コンボ完了
				if (bb.GetBool("PunchStarted"))
				{
					bb.SetBool("PunchStarted", false);
					return BTStatus::Success;
				}

				// 待機中 → 初撃入力
				input->attackInputType = AttackInputType::Attack1;
				return BTStatus::Running;
			}));

			attackProcess->AddChild(std::make_unique<BTAction>([](Blackboard& bb)
			{
				Enemy* enemy = bb.world->GetComponent<Enemy>(bb.self);
				enemy->attackCoolDownTimer = enemy->attackCoolDown;
				return BTStatus::Success;
			}));

			root->AddChild(std::move(attackProcess));
		}

		// 見合う (円周移動)
		{
			auto miaiAction = std::make_unique<BTAction>([](Blackboard& bb) 
			{
				auto* aiAgent = bb.world->GetComponent<AIAgent>(bb.self);
				auto* myT = bb.world->GetComponent<Transform>(bb.self);
				auto* targetT = bb.world->GetComponent<Transform>(bb.target);

				if (!aiAgent || !myT || !targetT) return BTStatus::Failure;

				aiAgent->speed = 100;

				float dx = myT->position.x - targetT->position.x;
				float dz = myT->position.z - targetT->position.z;
				float currentAngle = Mathf::Atan2(dz, dx);
				float radius = Mathf::Sqrt(dx * dx + dz * dz);


				float rotationSpeed = 0.01f;
				float nextAngle = currentAngle + rotationSpeed;

				float posX = targetT->position.x + radius * Mathf::Cos(nextAngle);
				float posZ = targetT->position.z + radius * Mathf::Sin(nextAngle);

				// 目的地を毎フレーム微更新する
				AIAgentSystem::GetInstance()->SetDestination(*aiAgent, Vector3(posX, myT->position.y, posZ));

				return BTStatus::Success;
			});

			root->AddChild(std::move(miaiAction));
		}

		return root;
	}
}