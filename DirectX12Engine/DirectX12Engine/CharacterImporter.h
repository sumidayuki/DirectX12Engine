#pragma once

class CharacterImporter
{
public:
	void Import();

private:
	void ProcessCharacterInfo(const Json& json);
	void ProcessStatus(const Json& json, CharacterInfo& info);
	void ProcessMoves(const Json& json, CharacterInfo& info);
	void ProcessMove(const Json& json, MoveData& move);
	void ProcessAttackParams(const Json& json, AttackParams& params);
	void ProcessGuardParams(const Json& json, GuardParams& params);
	void ProcessRollingParams(const Json& json, RollingParams& params);
};