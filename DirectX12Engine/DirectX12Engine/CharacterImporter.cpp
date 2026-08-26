#include "Precompiled.h"
#include "GuardState.h"
#include "CharacterInfoRegistry.h"
#include "CharacterImporter.h"


// ï∂éöóÒ Å® MoveType
std::map<std::string, MoveType> StringToMoveType =
{
	{ "Idle",		MoveType::Idle },
	{ "Attack",		MoveType::Attack },
	{ "Guard",		MoveType::Guard },
	{ "Rolling",	MoveType::Rolling }
};

// ï∂éöóÒ Å® MoveTransitionType
std::map<std::string, MoveTransitionType> StringToMoveTransitionType =
{
	{ "InputPressed",	MoveTransitionType::InputPressed },
	{ "InputReleased",	MoveTransitionType::InputReleased },
	{ "MoveEnd",			MoveTransitionType::MoveEnd }
};

// ï∂éöóÒ Å® ColliderType
std::map<std::string, ColliderType> StringToColliderType =
{
	{ "AABB", ColliderType::AABB },
	{ "Sphere", ColliderType::Sphere },
	{ "Box", ColliderType::Box }
};

void CharacterImporter::Import()
{
	for (const auto& entry : std::filesystem::directory_iterator("Assets/Json/Characters"))
	{
#if _DEBUG
		OutputDebugStringA(
			("Processing Character file: " +
				entry.path().string() +
				"\n").c_str());
#endif

		SetAssetPath(entry.path().wstring().c_str());

		if (GetExtension() != L".json")
		{
			continue;
		}

		std::ifstream file(GetAssetPath());

		if (!file.is_open())
		{
			assert(0);
			continue;
		}

		try
		{
			std::string text = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

			Json json = Json::parse(text, nullptr, true, true);

			ProcessCharacterInfo(json);
		}
		catch (const Json::exception& e)
		{
			std::ostringstream oss;

			oss << "JSONóvëfÇÃâêÕíÜÇ…ÉGÉâÅ[Ç™î≠ê∂: " << e.what() << "\n";

#if _DEBUG
			OutputDebugStringA(oss.str().c_str());
			assert(0);
#endif
		}
	}
}


void CharacterImporter::ProcessCharacterInfo(const Json& json)
{
	CharacterInfo info;

	info.name = UTF16LEtoUTF8::Convert(GetFileNameWithoutExtension());

	ProcessStatus(json, info);
	ProcessColliders(json, info);
	ProcessMoves(json, info);

	CharacterInfoRegistry::GetInstance()->RegisterCharacterInfo(info);

#if _DEBUG
	OutputDebugStringW((L"CharacterÇÃì«Ç›çûÇ›Ç…ê¨å˜: " + std::wstring(info.name.begin(), info.name.end()) + L"\n").c_str());
#endif
}

void CharacterImporter::ProcessStatus(const Json& json, CharacterInfo& info)
{
	const Json& status = json.value("status", Json::object());

	for (auto& [key, value] : status.items())
	{
		if (value.is_boolean())
		{
			StatusAPI::SetBool(info.status, key, value.get<bool>());
		}
		else if (value.is_number())
		{
			StatusAPI::SetFloat(info.status, key, value.get<float>());
		}
		else if (value.is_string())
		{
			StatusAPI::SetString(info.status, key, value.get<std::string>());
		}
		else if (value.is_object())
		{
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
	OutputDebugStringW(
		L"CharacterÇÃStatusì«Ç›çûÇ›Ç…ê¨å˜\n");
#endif
}

void CharacterImporter::ProcessColliders(const Json& json, CharacterInfo& info)
{
	const Json& collidersJson = json.value("colliders", Json::array());

	for (auto& colliderJson : collidersJson)
	{
		ColliderData colliderData;

		ProcessCollider(colliderJson, colliderData);

		info.colliders.push_back(colliderData);
	}
}

void CharacterImporter::ProcessCollider(const Json& json, ColliderData& colliderData)
{
	colliderData.name = json.value("name", "");

	colliderData.collider.type = StringToColliderType[json.value("type","")];

	const Json& params = json.value("params", Json::object());

	switch (colliderData.collider.type)
	{
	case ColliderType::Box:
	case ColliderType::AABB:

		colliderData.collider.size.x = params.value("sizeX", 0.0f);
		colliderData.collider.size.y = params.value("sizeY", 0.0f);
		colliderData.collider.size.z = params.value("sizeZ", 0.0f);

		break;

	case ColliderType::Sphere:

		colliderData.collider.radius = params.value("radius", 0.0f);

		break;
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
	const Json& moves = json.value("moves", Json::array());

	for (const auto& moveJson : moves)
	{
		MoveData move;

		ProcessMove(moveJson, move);

		info.moves.push_back(std::move(move));
	}

#if _DEBUG
	OutputDebugStringW(L"CharacterÇÃMovesì«Ç›çûÇ›Ç…ê¨å˜\n");
#endif
}

void CharacterImporter::ProcessMove(const Json& json, MoveData& move)
{
	move.moveId = FNV1a_Hash<uint32_t>(json.value("moveId", ""));
	move.textName = json.value("textName", "");
	move.duration = json.value("duration", 0.0f);
	move.staminaCost = json.value("staminaCost", 0.0f);

	const std::string typeName = json.value("type", "Idle");
	move.type = StringToMoveType[typeName];

	switch (move.type)
	{
		case MoveType::Idle:
		{
			move.params = IdleParams{};
			break;
		}

		case MoveType::Attack:
		{
			AttackParams params;
			ProcessAttackParams(json, params);
			move.params = std::move(params);
			break;
		}

		case MoveType::Guard:
		{
			GuardParams params;
			ProcessGuardParams(json, params);
			move.params = std::move(params);
			break;
		}

		case MoveType::Rolling:
		{
			RollingParams params;
			ProcessRollingParams(json, params);
			move.params = std::move(params);
			break;
		}
	}

	ProcessTransitions(json, move);

#if _DEBUG
	OutputDebugStringW((L"CharacterÇÃMoveì«Ç›çûÇ›Ç…ê¨å˜: MoveID = " + std::to_wstring(move.moveId) + L"\n").c_str());
#endif
}

void CharacterImporter::ProcessTransitions(const Json& json, MoveData& move)
{
	const Json& transitions = json.value("transitions", Json::array());

	for (const auto& transitionJson : transitions)
	{
		MoveTransition transition;
		ProcessTransition(transitionJson, transition);
		move.transitions.push_back(transition);
	}
}

void CharacterImporter::ProcessTransition(const Json& json, MoveTransition& transition)
{
	const std::string targetName = json.value("target", "");
	transition.targetMoveId = FNV1a_Hash<uint32_t>(targetName.c_str());

	const std::string typeName = json.value("type", "InputPressed");
	transition.type = StringToMoveTransitionType[typeName];

	if (json.contains("inputKey"))
	{
		const std::string inputKeyName = json.at("inputKey").get<std::string>();
		transition.inputKey = StringToInputKey[inputKeyName];
	}

	transition.start = json.value("start", 0.0f);
	transition.end = json.value("end", 1.0f);
}

void CharacterImporter::ProcessAttackParams(const Json& json, AttackParams& params)
{
	const Json& attackParams = json.value("params", Json::object());

	params.animationName = attackParams.value("animationName", "");
	params.damage = attackParams.value("damage", 0.0f);
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

	params.animationName = rollingParams.value("animationName", "");
	params.invincibleStart = rollingParams.value("invincibleStart", 0.0f);
	params.invincibleEnd = rollingParams.value("invincibleEnd", 0.0f);
	params.moveSpeed = rollingParams.value("moveSpeed", 0.0f);
}