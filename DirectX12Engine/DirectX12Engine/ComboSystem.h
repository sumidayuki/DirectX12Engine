#pragma once

class ComboSystem : public System
{
private:

	void TransionTo(ComboState& state, int newMoveID);

	void ResetCombo(ComboState& state);

	void ClearInput(ComboInput& input);

	int GetNextMoveID(const std::string& name, AttackInputType type, const std::vector<int>& possibles);

private:
	void Update(World& world) override;
};