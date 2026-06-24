#include "Precompiled.h"
#include "CharacterImporter.h"
#include "GuardState.h"

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
		std::ostringstream oss;
		oss << "JSON要素の解析中にエラーが発生: " << e.what() << "\n";
		OutputDebugStringA(oss.str().c_str());
		assert(0);
	}
}

bool CharacterImporter::CharcterInitialize(const std::string& name, Entity entity, World& world)
{
	const CharacterInfo& info = m_characterInfos.at(name);

	// CharacterStatus コンポーネントを付与
	world.AddComponent<CharacterStatus>(entity, info.status);

	world.AddComponent<Velocity>(entity, Velocity{});
	HP playerHP;
	playerHP.maxHP = StatusAPI::GetFloat(info.status, "maxHealth", 100.0f);
	world.AddComponent<HP>(entity, playerHP);
	world.AddComponent<Damageable>(entity, Damageable{});
	world.AddComponent<Rigidbody>(entity, Rigidbody{});
	Collider bColl;
	bColl.type = ColliderType::AABB;
	float hitBoxX = StatusAPI::GetFloat(info.status, "hitBoxX", 40.0f);
	float hitBoxY = StatusAPI::GetFloat(info.status, "hitBoxY", 180.0f);
	float hitBoxZ = StatusAPI::GetFloat(info.status, "hitBoxZ", 40.0f);
	bColl.size = Vector3(hitBoxX, hitBoxY, hitBoxZ);
	bColl.offset = Vector3(0, hitBoxY * 0.5f, 0);
	world.AddComponent<Collider>(entity, bColl);
	ComboState comboState;
	comboState.name = name;
	world.AddComponent<ComboState>(entity, comboState);
	ComboInput comboInput;
	world.AddComponent<ComboInput>(entity, comboInput);
	GuardState guardState;
	float shieldMax = StatusAPI::GetFloat(info.status, "shieldMaxHealth", 100.0f);
	guardState.shieldMaxHealth = shieldMax;
	guardState.shieldHealth = shieldMax;
	world.AddComponent<GuardState>(entity, guardState);
	Attackable attackable;
	world.AddComponent<Attackable>(entity, attackable);

	return true;
}

const CharacterInfo* CharacterImporter::GetCharacterInfo(const std::string& name) const
{
	// キャラクター情報が存在するか確認
	auto it = m_characterInfos.find(name);
	if (it != m_characterInfos.end())
	{
		return &it->second;
	}

	return nullptr;
}

const ComboMove& CharacterImporter::GetMoveById(const std::string& name, int id) const
{
	for (const auto& move : m_characterInfos.at(name).moves)
	{
		if (move.moveId == id)
		{
			return move;
		}
	}
	throw std::runtime_error("Move ID not found");
}

void CharacterImporter::ProcessCharacterInfo(const Json& json)
{
	for (auto& [characterName, data] : json.items())
	{
		CharacterInfo info;
		info.name = characterName;

		ProcessStatus(data, info);
		ProcessMoves(data, info);

		m_characterInfos[characterName] = info;
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
			// Vector3 として解釈 { "x": ..., "y": ..., "z": ... }
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
		ComboMove move;
		ProcessMove(moveJson, move);
		info.moves.push_back(move);
	}

#if _DEBUG
	OutputDebugStringW((L"CharacterのMoves読み込みに成功\n"));
#endif
}

void CharacterImporter::ProcessMove(const Json& json, ComboMove& move)
{
	move.moveId = json.at("moveId").get<int>();
	move.textName = json.value("textName", "");
	move.attackType = json.at("attackType").get<int>();
	move.isStarter = json.at("isStarter").get<bool>();
	move.requiredInput = (AttackInputType)json.at("requiredInput").get<int>();
	move.duration = json.at("duration").get<float>();
	move.inputStart = json.at("inputStart").get<float>();
	move.inputEnd = json.at("inputEnd").get<float>();
	move.hitStartTime = json.at("hitStartTime").get<float>();
	move.hitEndTime = json.at("hitEndTime").get<float>();
	move.damage = json.at("damage").get<float>();
	move.animationName = json.at("animationName").get<std::string>();
	move.nextPossibleMoves = json.at("nextPossibleMoves").get<std::vector<int>>();

#if _DEBUG
	OutputDebugStringW((L"CharacterのMove読み込みに成功: MoveID = " + std::to_wstring(move.moveId) + L"\n").c_str());
#endif
}
