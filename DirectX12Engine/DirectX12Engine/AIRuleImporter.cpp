#include "AIRuleImporter.h"
#include "AIRuleRegistry.h"

void AIRuleImporter::Import()
{
	// Assets/Json/AI/の全てのJSONファイルを読み込む
	for(const auto& entry : std::filesystem::directory_iterator("Assets/Json/AI"))
	{
		OutputDebugStringA(("Processing AI rule file: " + entry.path().string() + "\n").c_str());

		SetAssetPath(entry.path().wstring().c_str());

		if (GetExtension() == L".json")
		{
			std::ifstream file(GetAssetPath());
			if (!file.is_open())
			{
				assert(0);
				continue;
			}
			try
			{
				std::string text = std::string((std::istreambuf_iterator<char>(file)),
					std::istreambuf_iterator<char>());
				Json json = Json::parse(text, nullptr, true, true);
				ProcessRules(json);
			}
			catch (const Json::exception& e)
			{
				std::ostringstream oss;
				oss << "JSON要素の解析中にエラーが発生: " << e.what() << "\n";
				OutputDebugStringA(oss.str().c_str());
				assert(0);
			}
		}
	}
}

void AIRuleImporter::ProcessRules(const Json& json)
{
	for(auto& [stateName, transitionsJson] : json.items())
	{
		ProcessTransitions(transitionsJson, stateName);
	}
}

void AIRuleImporter::ProcessTransitions(const Json& json, const std::string& stateName)
{
	for(const auto& transitionJson : json)
	{
		ProcessTransition(transitionJson, stateName);
	}
}

void AIRuleImporter::ProcessTransition(const Json& json, const std::string& stateName)
{
	Transition transition;

	transition.trigger = FNV1a_Hash<uint32_t>(json.at("Trigger").get<std::string>().c_str());

	std::string toName = json.at("To").get<std::string>();
	transition.nextState = FNV1a_Hash<uint32_t>(toName.c_str());
	transition.nextStateName = toName;

	if (json.contains("Input"))
	{
		std::string inputStr = json.at("Input").get<std::string>();
		transition.input = StringToInputKey[inputStr];
	}

	for (auto& condJson : json.at("Conditions"))
	{
		ProcessCondition(condJson, transition);
	}

	uint32_t characterId = FNV1a_Hash<uint32_t>(UTF16LEtoUTF8::Convert(GetFileNameWithoutExtension()).c_str());
	uint32_t stateId = FNV1a_Hash<uint32_t>(stateName.c_str());
	AIRuleRegistry::GetInstance()->RegisterRule(characterId, stateId, transition);
}

void AIRuleImporter::ProcessCondition(const Json& json, Transition& transition)
{
	std::string type = json.at("Type").get<std::string>();
	uint32_t typeHash = FNV1a_Hash<uint32_t>(type.c_str());

	bool invert = json.value("Invert", false);

	switch (typeHash)
	{
	case "PlayerInRange"_h:
	{
		// JSON から判定距離を取得（ラムダのキャプチャに使う）
		float range = json.at("Range").get<float>();

		transition.conditions.push_back([range, invert](Entity entity, World& world) {
			// 自分（Enemy）からターゲットの位置を取得
			auto* enemy = world.GetComponent<Enemy>(entity);
			auto* myTrans = world.GetComponent<Transform>(entity);
			auto* targetTrans = world.GetComponent<Transform>(enemy->target);

			// 2点間の距離を計算して、Range と比較する
			bool result = (targetTrans->position - myTrans->position).Magnitude() < range;
			return invert ? !result : result;
			});
		break;
	}

	case "CoolDownReady"_h:
	{
		std::string timerName = json.at("TimerName").get<std::string>();

		transition.conditions.push_back([timerName, invert](Entity entity, World& world) {
			CharacterStatus* status = world.GetComponent<CharacterStatus>(entity);

			// クールダウンタイマーが0以下なら攻撃可能
			bool result = StatusAPI::GetFloat(*status, timerName) <= 0.0f;
			return invert ? !result : result;
			});
		break;
	}

	case "IsTargetDead"_h:
	{
		transition.conditions.push_back([invert](Entity entity, World& world) {
			auto* enemy = world.GetComponent<Enemy>(entity);

			// ターゲットの HP コンポーネントを取得
			auto* targetHP = world.GetComponent<HP>(enemy->target);

			// isDeath フラグで生存判定
			bool result = targetHP && targetHP->isDeath;
			return invert ? !result : result;
			});
		break;
	}

	case "IsTargetHit"_h:
	{
		transition.conditions.push_back([invert](Entity entity, World& world) {
			Enemy* enemy = world.GetComponent<Enemy>(entity);
			Animator* animator = world.GetComponent<Animator>(enemy->target);

			return animator->currentClipName == "Hit_00";
		});
		break;
	}

	case "ShouldContinuePunchCombo"_h:
	{
		transition.conditions.push_back([invert](Entity entity, World& world) {
			Enemy* enemy = world.GetComponent<Enemy>(entity);
			MoveState* state = world.GetComponent<MoveState>(entity);
			GuardState* targetGuard = world.GetComponent<GuardState>(enemy->target);
			Animator* targetAnimator = world.GetComponent<Animator>(enemy->target);

			// 相手がガード中又はダメージ食らっているなら4回まで、通常なら2回までコンボを繋ぐ
			int maxCombo = (targetGuard && targetGuard->isGuarding) || (targetAnimator && targetAnimator->currentClipName == "Hit_00" && targetAnimator->isPlaying) ? 4 : 2;
			bool result = state->comboIndex < maxCombo;
			return invert ? !result : result;
			});
		break;
	}

	case "UnderJumpComboCount"_h:
	{
		int maxCount = json.at("Max").get<int>();

		transition.conditions.push_back([maxCount, invert](Entity entity, World& world) {
			MoveState* state = world.GetComponent<MoveState>(entity);

			// 現在のジャンプコンボ回数を取得し、上限と比較する
			bool result = state->comboIndex < maxCount;
			return invert ? !result : result;
			});
		break;
	}

	case "IsSelfStunned"_h:
	{
		transition.conditions.push_back([invert](Entity entity, World& world) {
			auto* animator = world.GetComponent<Animator>(entity);
			auto* status = world.GetComponent<CharacterStatus>(entity);

			// 攻撃後の硬直タイマーが残っているか
			bool isRecovery = StatusAPI::GetFloat(*status, "RecoveryTimer") > 0.0f;

			return invert ? !isRecovery : isRecovery;
			});
		break;
	}
	}
}
