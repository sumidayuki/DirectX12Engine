#include "CharacterInfoRegistry.h"

void CharacterInfoRegistry::RegisterCharacterInfo(const CharacterInfo& info)
{
	m_characterInfos[info.name] = info;

#if _DEBUG
	OutputDebugStringW((L"CharacterInfoが登録されました: " + std::wstring(info.name.begin(), info.name.end()) + L"\n").c_str());
#endif
}

const CharacterInfo* CharacterInfoRegistry::GetCharacterInfo(const std::string& name) const
{
	auto it = m_characterInfos.find(name);
	if (it != m_characterInfos.end())
	{
		return &it->second;
	}

#if _DEBUG
	OutputDebugStringW((L"CharacterInfoが見つかりません: " + std::wstring(name.begin(), name.end()) + L"\n").c_str());
#endif
	return nullptr;
}

bool CharacterInfoRegistry::CharcterInitialize(const std::string& name, Entity entity, World& world)
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
	MoveState moveState;
	moveState.name = name;
	world.AddComponent<MoveState>(entity, moveState);
	MoveInput comboInput;
	world.AddComponent<MoveInput>(entity, comboInput);
	GuardState guardState;
	float shieldMax = StatusAPI::GetFloat(info.status, "shieldMaxHealth", 100.0f);
	guardState.shieldMaxHealth = shieldMax;
	guardState.shieldHealth = shieldMax;
	world.AddComponent<GuardState>(entity, guardState);
	Attackable attackable;
	world.AddComponent<Attackable>(entity, attackable);
	
	if (!info.colliders.empty())
	{
		CharacterHitboxes hitboxes;
		for (const auto& colliderData : info.colliders)
		{
			Entity colliderE = world.CreateEntity((colliderData.name + "_collider"));
			BoneSocket socket;
			socket.targetEntity = entity;
			socket.targetBoneName = colliderData.name;
			world.AddComponent<BoneSocket>(colliderE, socket);
			Collider* collider = world.AddComponent<Collider>(colliderE, colliderData.collider);
			collider->isEnable = false;
			
			hitboxes.entities[HashString32(colliderData.name.c_str())] = colliderE;
#if _DEBUG
			OutputDebugStringW((L"CharacterのColliderを作成: " + std::wstring(colliderE.name.begin(), colliderE.name.end()) + L"\n").c_str());
#endif
		}
		world.AddComponent<CharacterHitboxes>(entity, hitboxes);
	}

#if _DEBUG
	OutputDebugStringW((L"Characterの初期化に成功: " + std::wstring(name.begin(), name.end()) + L"\n").c_str());
#endif

	return true;
}

const MoveData& CharacterInfoRegistry::GetMoveById(const std::string& name, int id) const
{
	for (const auto& move : m_characterInfos.at(name).moves)
	{
		if (move.moveId == id)
		{
			return move;
		}
	}

#if _DEBUG
	OutputDebugStringW((L"CharacterのMoveが見つかりません: MoveID = " + std::to_wstring(id) + L"\n").c_str());
#endif
}
