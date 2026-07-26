#pragma once

struct ColliderData
{
	std::string name;
	Collider collider;
};

struct CharacterInfo
{
	std::string name;
	std::string aiFileName;
	bool isPlayer = false;
	CharacterStatus status;
	std::vector<ColliderData> colliders;
	std::vector<MoveData> moves;
};

class CharacterInfoRegistry : public Singleton<CharacterInfoRegistry>
{
	friend class Singleton<CharacterInfoRegistry>;

private:
	std::unordered_map<std::string, CharacterInfo> m_characterInfos;

public:
	void RegisterCharacterInfo(const CharacterInfo& info);

	const CharacterInfo* GetCharacterInfo(const std::string& name) const;

	bool CharcterInitialize(const std::string& name, Entity entity, World& world);

	const MoveData& GetMoveById(const std::string& name, int id) const;
};