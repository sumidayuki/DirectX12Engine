#pragma once

struct CharacterInfo
{
	std::string name;

	std::string aiFileName;

	CharacterStatus status;

	std::vector<ComboMove> moves;
};

class CharacterImporter : public Singleton<CharacterImporter>
{
	friend class Singleton<CharacterImporter>;

private:
	std::unordered_map<std::string, CharacterInfo> m_characterInfos;

public:
	void Import();

	bool CharcterInitialize(const std::string& name, Entity entity, World& world);

	const CharacterInfo* GetCharacterInfo(const std::string& name) const;

	const ComboMove& GetMoveById(const std::string& name, int id) const;

private:
	void ProcessCharacterInfo(const Json& json);
	void ProcessStatus(const Json& json, CharacterInfo& info);
	void ProcessMoves(const Json& json, CharacterInfo& info);
	void ProcessMove(const Json& json, ComboMove& move);
};