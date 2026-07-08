#include "Precompiled.h"
#include "CharacterImporter.h"
#include "GuardState.h"
#include "CharacterInfoRegistry.h"

std::map<std::string, MoveType> StringToMoveType = {
	{"Attack", MoveType::Attack},
	{"Guard", MoveType::Guard},
	{"Rolling", MoveType::Rolling}
};

void CharacterImporter::Import()
{
	std::ifstream file("Assets/Json/Characters.json");

	if (!file.is_open())
	{
		assert(0);

		// ファイルが開けない場合のエラー処理
		return;
	}

	try
	{
		// Json全体をパースしてルートオブジェクトを取得
		std::string text = std::string((std::istreambuf_iterator<char>(file)),
			std::istreambuf_iterator<char>());

		Json json = Json::parse(text);

		ProcessCharacterInfo(json);
	}
	catch (const Json::exception& e)
	{
#if _DEBUG
		std::ostringstream oss;
		oss << "JSON要素の解析中にエラーが発生: " << e.what() << "\n";
		OutputDebugStringA(oss.str().c_str());
#endif
		assert(0);
	}
}


void CharacterImporter::ProcessCharacterInfo(const Json& json)
{
	for (auto& [characterName, data] : json.items())
	{
		CharacterInfo info;
		info.name = characterName;

		ProcessStatus(data, info);
		ProcessMoves(data, info);

		CharacterInfoRegistry::GetInstance()->RegisterCharacterInfo(info);

#if _DEBUG
		OutputDebugStringW((L"Characterの読み込みに成功: " + std::wstring(characterName.begin(), characterName.end()) + L"\n").c_str());
#endif
	}
}

void CharacterImporter::ProcessStatus(const Json& json, CharacterInfo& info)
{
	const Json& status = json.value("status", Json::object());

	// JSON の型に応じて自動的に適切なマップに格納
	for (auto& [key, value] : status.items())
	{
		if (value.is_boolean())
		{
			StatusAPI::SetBool(info.status, key, value.get<bool>());
		}
		else if (value.is_number())
		{
			// nlohmann JSON では 150.0 のような値も integer として判定される場合があるため
			// 数値は全て float として格納する
			StatusAPI::SetFloat(info.status, key, value.get<float>());
		}
		else if (value.is_string())
		{
			StatusAPI::SetString(info.status, key, value.get<std::string>());
		}
		else if (value.is_object())
		{
			// Vector3 として変換
			if (value.contains("x") && value.contains("y") && value.contains("z"))
			{
				Vector3 v;
				v.x = value.value("x", 0.0f);
				v.y = value.value("y", 0.0f);
				v.z = value.value("z", 0.0f);
				StatusAPI::SetVector(info.status, key, v);
			}
		}
	}

#if _DEBUG
	OutputDebugStringW((L"CharacterのStatus読み込みに成功\n"));
#endif
}

void CharacterImporter::ProcessMoves(const Json& json, CharacterInfo& info)
{
	const Json& moves = json.value("moves", Json::array());

	for (const auto& moveJson : moves)
	{
		MoveData move;
		ProcessMove(moveJson, move);
		info.moves.push_back(move);
	}

#if _DEBUG
	OutputDebugStringW((L"CharacterのMoves読み込みに成功\n"));
#endif
}

void CharacterImporter::ProcessMove(const Json& json, MoveData& move)
{
	move.moveId = FNV1a_Hash<uint32_t>(json.value("moveId", ""));
	move.textName = json.value("textName", "");
	std::string inputKeyStr = json.value("inputKey", "");
	move.inputKey = StringToInputKey[inputKeyStr];
	move.isStarter = json.value("isStarter", false);
	std::vector<std::string> nextMoves = json.value("nextPossibleMoves", std::vector<std::string>());
	for (const auto& nextMove : nextMoves)
	{
		move.nextPossibleMoves.push_back(FNV1a_Hash<uint32_t>(nextMove));
	}
	move.duration = json.value("duration", 0.0f);
	move.animationName = json.value("animationName", "");
	move.type = StringToMoveType[json.value("type", "")];

	switch (move.type)
	{
		case MoveType::Attack:
		{
			AttackParams params;
			ProcessAttackParams(json, params);
			move.params = params;
			break;
		}

		case MoveType::Guard:
		{
			GuardParams params;
			ProcessGuardParams(json, params);
			move.params = params;
			break;
		}

		case MoveType::Rolling:
		{
			RollingParams params;
			ProcessRollingParams(json, params);
			move.params = params;
			break;
		}
	}

#if _DEBUG
	OutputDebugStringW((L"CharacterのMove読み込みに成功: MoveID = " + std::to_wstring(move.moveId) + L"\n").c_str());
#endif
}

void CharacterImporter::ProcessAttackParams(const Json& json, AttackParams& params)
{
	const Json& attackParams = json.value("params", Json::object());
	params.allowCancel = attackParams.value("allowCancel", false);
	params.damage = attackParams.value("damage", 0.0f);
	params.inputStart = attackParams.value("inputStart", 0.0f);
	params.inputEnd = attackParams.value("inputEnd", 0.0f);
	params.hitStartTime = attackParams.value("hitStartTime", 0.0f);
	params.hitEndTime = attackParams.value("hitEndTime", 0.0f);
	params.colliderNames = attackParams.value("colliderNames", std::vector<std::string>());
}

void CharacterImporter::ProcessGuardParams(const Json& json, GuardParams& params)
{
}

void CharacterImporter::ProcessRollingParams(const Json& json, RollingParams& params)
{
	const Json& rollingParams = json.value("params", Json::object());
	params.invincibleStart = rollingParams.value("invincibleStart", 0.0f);
	params.invincibleEnd = rollingParams.value("invincibleEnd", 0.0f);
	params.moveSpeed = rollingParams.value("moveSpeed", 0.0f);
}