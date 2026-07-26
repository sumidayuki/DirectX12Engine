#pragma once

class MoveSystem : public System
{
private:
	void TransitionTo(MoveState& state, uint32_t newMoveID);
	void ResetMove(MoveState& state);
	void ClearInput(MoveInput& input);
	uint32_t GetNextMoveID(const std::string& name, InputKey input, const std::vector<uint32_t>& possibles);

private:
	void Update(World& world) override;
};