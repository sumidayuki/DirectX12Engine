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

	world.AddComponent<Velocity>(entity, Velocity{});
	HP playerHP;
	playerHP.maxHP = info.maxHealth;
	world.AddComponent<HP>(entity, playerHP);
	world.AddComponent<Damageable>(entity, Damageable{});
	world.AddComponent<Rigidbody>(entity, Rigidbody{});
	Collider bColl;
	bColl.type = ColliderType::AABB;
	bColl.size = Vector3(info.hitBoxX, info.hitBoxY, info.hitBoxZ);
	bColl.offset = Vector3(0, info.hitBoxY * 0.5f, 0);
	world.AddComponent<Collider>(entity, bColl);
	ComboState comboState;
	comboState.name = name;
	world.AddComponent<ComboState>(entity, comboState);
	ComboInput comboInput;
	world.AddComponent<ComboInput>(entity, comboInput);
	GuardState guardState;
	guardState.shieldMaxHealth = info.shieldMaxHealth;
	guardState.shieldHealth = info.shieldMaxHealth;
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

	info.maxHealth = status.value("maxHealth", 100.0f);
	info.walkSpeed = status.value("walkSpeed", 100.0f);
	info.runSpeed = status.value("runSpeed", 250.0f);
	info.hitBoxX = status.value("hitBoxX", 40.0f);
	info.hitBoxY = status.value("hitBoxY", 180.0f);
	info.hitBoxZ = status.value("hitBoxZ", 40.0f);
	info.shieldMaxHealth = status.value("shieldMaxHealth", 100.0f);
	info.shieldRegenRate = status.value("shieldRegenRate", 10.0f);

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
