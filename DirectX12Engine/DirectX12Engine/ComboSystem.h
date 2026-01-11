#pragma once

class ComboSystem : public System
{
private:
	std::unordered_map<std::string, std::vector<ComboMove>> m_moves;

private:
	/// <summary>
	/// 指定のキャラクター名のMoveIDからMoveを取得します。
	/// </summary>
	/// <param name="id"></param>
	/// <returns></returns>
	const ComboMove& GetMoveById(const std::string& name, int id) const;

	void TransionTo(ComboState& state, int newMoveID);

	void ResetCombo(ComboState& state);

	void ClearInput(ComboInput& input);

	int GetNextMoveID(const std::string& name, AttackInputType type, const std::vector<int>& possibles);

private:
	bool Load(World& world) override;
	void Update(World& world) override;
};