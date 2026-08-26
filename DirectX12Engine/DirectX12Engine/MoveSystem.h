#pragma once

class MoveSystem : public System
{
private:
	const MoveData* GetTransitionTarget(const std::string& name, const MoveTransition& transition, const Stamina& stamina) const;

	void TransitionTo(MoveState& state, const MoveData& targetMove, Stamina& stamina);
	void ClearPressedInput(MoveInput& input);

	bool IsTransitionMatched(const MoveTransition& transition, const MoveInput& input, float progress) const;

	bool TryInputTransition(MoveState& state, MoveInput& input, Stamina& stamina, const MoveData& currentMove, float progress);
	bool TryEndTransition(Entity entity, World& world, MoveState& state, MoveInput& input, Stamina& stamina, const MoveData& currentMove, float progress);

	void ProcessAttack(Entity entity, World& world, MoveState& state, const MoveData& currentMove, float progress);
	void SetAttackHitboxesEnabled(Entity entity, World& world, const AttackParams& params, bool enabled);

private:
	void Update(World& world) override;
};