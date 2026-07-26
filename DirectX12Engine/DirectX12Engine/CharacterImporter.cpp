#include "Precompiled.h"
#include "GuardState.h"
#include "CharacterInfoRegistry.h"
#include "CharacterImporter.h"

// 文字列からMoveType列挙型へのマッピング
std::map<std::string, MoveType> StringToMoveType = {
	{"Attack", MoveType::Attack},
	{"Guard", MoveType::Guard},
	{"Rolling", MoveType::Rolling}
};

// 文字列からColliderType列挙型へのマッピング
std::map<std::string, ColliderType> StringToColliderType = {
	{"AABB", ColliderType::AABB},
	{"Sphere", ColliderType::Sphere},
	{"Box", ColliderType::Box}
};

void CharacterImporter::Import()
{
	// Assets/Json/Characters/の全てのJSONファイルを読み込む
	for(const auto& entry : std::filesystem::directory_iterator("Assets/Json/Characters"))
	{
#if _DEBUG
		OutputDebugStringA(("Processing Character file: " + entry.path().string() + "\n").c_str());
#endif

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
				ProcessCharacterInfo(json);
			}
			catch (const Json::exception& e)
			{
				std::ostringstream oss;
				oss << "JSON要素の解析中にエラーが発生: " << e.what() << "\n";
#if _DEBUG
				OutputDebugStringA(oss.str().c_str());
				assert(0);
#endif
			}
		}
	}
}


void CharacterImporter::ProcessCharacterInfo(const Json& json)
{
	// キャラクターインフォの作成
	CharacterInfo info;
	info.name = UTF16LEtoUTF8::Convert(GetFileNameWithoutExtension());

	ProcessStatus(json, info);
	ProcessColliders(json, info);
	ProcessMoves(json, info);

	// レジストリに登録
	CharacterInfoRegistry::GetInstance()->RegisterCharacterInfo(info);

#if _DEBUG
	OutputDebugStringW((L"Characterの読み込みに成功: " + std::wstring(info.name.begin(), info.name.end()) + L"\n").c_str());
#endif
}

void CharacterImporter::ProcessStatus(const Json& json, CharacterInfo& info)
{
	// キャラクターのステータス情報を読み込む
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

void CharacterImporter::ProcessColliders(const Json& json, CharacterInfo& info)
{
	const Json& collidersJson = json.value("colliders", Json::array());

	for(auto& colliderJson : collidersJson)
	{
		ColliderData colliderData;
		ProcessCollider(colliderJson, colliderData);
		info.colliders.push_back(colliderData);
	}
}

void CharacterImporter::ProcessCollider(const Json& json, ColliderData& colliderData)
{
	colliderData.name = json.value("name", "");
	colliderData.collider.type = StringToColliderType[json.value("type", "")];
	const Json& params = json.value("params", Json::object());
	switch (colliderData.collider.type)
	{
	case ColliderType::Box:
	case ColliderType::AABB:
		colliderData.collider.size.x = params.value("sizeX", 0.0f);
		colliderData.collider.size.y = params.value("sizeY", 0.0f);
		colliderData.collider.size.z = params.value("sizeZ", 0.0f);

	case ColliderType::Sphere:
		colliderData.collider.radius = params.value("radius", 0.0f);
	}
	colliderData.collider.offset.x = params.value("offsetX", 0.0f);
	colliderData.collider.offset.y = params.value("offsetY", 0.0f);
	colliderData.collider.offset.z = params.value("offsetZ", 0.0f);
	colliderData.collider.isEnable = params.value("isEnable", true);
	colliderData.collider.isTrigger = params.value("isTrigger", false);
	colliderData.collider.collisionMask = StringToLayerMask[params.value("collisionMask", "")];
}

void CharacterImporter::ProcessMoves(const Json& json, CharacterInfo& info)
{
	// キャラクターのムーブ情報を読み込む
	const Json& moves = json.value("moves", Json::array());

	for (const auto& moveJson : moves)
	{
		// ムーブ情報を作成
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
	move.duration = json.value("duration", 0.0f);
	move.isStarter = json.value("isStarter", false);
	move.staminaCost = json.value("staminaCost", 0.0f);
	std::vector<std::string> nextMoves = json.value("nextPossibleMoves", std::vector<std::string>());
	for (const auto& nextMove : nextMoves)
	{
		move.nextPossibleMoves.push_back(FNV1a_Hash<uint32_t>(nextMove));
	}
	move.type = StringToMoveType[json.value("type", "")];

	// ムーブの種類に応じたパラメータを処理
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
	params.animationName = attackParams.value("animationName", "");
	params.damage = attackParams.value("damage", 0.0f);
	params.inputStart = attackParams.value("inputStart", 0.0f);
	params.inputEnd = attackParams.value("inputEnd", 0.0f);
	params.hitStartTime = attackParams.value("hitStartTime", 0.0f);
	params.hitEndTime = attackParams.value("hitEndTime", 0.0f);
	params.colliderNames = attackParams.value("colliderNames", std::vector<std::string>());
}

void CharacterImporter::ProcessGuardParams(const Json& json, GuardParams& params)
{
	// ガードに必要なパラメーターが現在ないので、空のままにしておく
}

void CharacterImporter::ProcessRollingParams(const Json& json, RollingParams& params)
{
	const Json& rollingParams = json.value("params", Json::object());
	params.animationName = rollingParams.value("animationName", "");
	params.invincibleStart = rollingParams.value("invincibleStart", 0.0f);
	params.invincibleEnd = rollingParams.value("invincibleEnd", 0.0f);
	params.moveSpeed = rollingParams.value("moveSpeed", 0.0f);
}