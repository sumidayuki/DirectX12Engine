#pragma once

class ComboSystem : public System
{
private:

	void TransionTo(ComboState& state, uint32_t newMoveID);

	void ResetCombo(ComboState& state);

	void ClearInput(ComboInput& input);

	uint32_t GetNextMoveID(const std::string& name, InputKey input, const std::vector<uint32_t>& possibles);

private:
	void Update(World& world) override;
};