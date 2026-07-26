#pragma once
#include "CharacterInfoRegistry.h"

class CharacterImporter : public AssetImporter
{
public:
	/// <summary>
	/// Assets/Json/Characters/の全てのJSONファイルを読み込み、CharacterInfoRegistryに登録します。
	/// </summary>
	void Import();

private:
	void ProcessCharacterInfo(const Json& json);
	void ProcessStatus(const Json& json, CharacterInfo& info);
	void ProcessColliders(const Json& json, CharacterInfo& info);
	void ProcessCollider(const Json& json, ColliderData& colliderData);
	void ProcessMoves(const Json& json, CharacterInfo& info);
	void ProcessMove(const Json& json, MoveData& move);
	void ProcessAttackParams(const Json& json, AttackParams& params);
	void ProcessGuardParams(const Json& json, GuardParams& params);
	void ProcessRollingParams(const Json& json, RollingParams& params);
};