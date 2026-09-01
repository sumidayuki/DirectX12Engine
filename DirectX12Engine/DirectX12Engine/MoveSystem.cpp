#include "Precompiled.h"
#include "MoveSystem.h"
#include "MoveState.h"
#include "MoveInput.h"
#include "CharacterStatus.h"
#include "CharacterImporter.h"

const MoveData* MoveSystem::GetTransitionTarget(const std::string& name, const MoveTransition& transition, const Stamina& stamina) const
{
	const MoveData& targetMove = CharacterInfoRegistry::GetInstance()->GetMoveById(name, transition.targetMoveId);

	if (targetMove.staminaCost > 0.0f)
	{
		if (stamina.value < targetMove.staminaCost)
		{
			return nullptr;
		}
	}

	return &targetMove;
}

void MoveSystem::TransitionTo(MoveState& state, const MoveData& targetMove, Stamina& stamina)
{
	if (targetMove.staminaCost > 0.0f)
	{
		stamina.value -= targetMove.staminaCost;
		stamina.timer = 0.0f;
	}

	if (targetMove.type == MoveType::Idle)
	{
		state.comboIndex = 0;
	}
	else
	{
		state.comboIndex++;
	}

	state.currentMoveId = targetMove.moveId;
	state.hitConfirm = false;
	state.canHit = false;
	state.isAnimed = false;
	state.timer = 0.0f;
}

void MoveSystem::ClearPressedInput(MoveInput& input)
{
	input.inputKey = InputKey::None;
	input.timer = 0.0f;
}

bool MoveSystem::IsTransitionMatched(const MoveTransition& transition, const MoveInput& input, float progress) const
{
	switch (transition.type)
	{
		case MoveTransitionType::InputPressed:
			return input.inputKey == transition.inputKey &&
				progress >= transition.start &&
				progress <= transition.end;

		case MoveTransitionType::InputReleased:
			return input.releasedKey == transition.inputKey &&
				progress >= transition.start &&
				progress <= transition.end;

		case MoveTransitionType::MoveEnd:
			return progress >= 1.0f;
	}

	return false;
}

bool MoveSystem::TryInputTransition(MoveState& state, MoveInput& input, Stamina& stamina, const MoveData& currentMove, float progress)
{
	for (const auto& transition : currentMove.transitions)
	{
		if (transition.type == MoveTransitionType::MoveEnd)
		{
			continue;
		}

		if (!IsTransitionMatched(transition, input, progress))
		{
			continue;
		}

		const MoveData* targetMove = GetTransitionTarget(state.name, transition, stamina);

		// スタミナ不足などで遷移先を取得できなかった場合
		if (!targetMove)
		{
			continue;
		}

		TransitionTo(state, *targetMove, stamina);

		if (transition.type == MoveTransitionType::InputPressed)
		{
			ClearPressedInput(input);
		}
		else if (transition.type == MoveTransitionType::InputReleased)
		{
			input.releasedKey = InputKey::None;
		}

		return true;
	}

	return false;
}

bool MoveSystem::TryEndTransition(Entity entity, World& world, MoveState& state, MoveInput& input, Stamina& stamina, const MoveData& currentMove, float progress)
{
	if (currentMove.duration <= 0.0f)
	{
		return false;
	}

	for (const auto& transition : currentMove.transitions)
	{
		if (transition.type != MoveTransitionType::MoveEnd)
		{
			continue;
		}

		if (!IsTransitionMatched(transition, input, progress))
		{
			continue;
		}

		const MoveData* targetMove = GetTransitionTarget(state.name, transition, stamina);

		if (!targetMove)
		{
			continue;
		}

		if (currentMove.type == MoveType::Attack)
		{
			AITrigger* trigger = world.GetComponent<AITrigger>(entity);

			if (trigger)
			{
				trigger->triggers.push_back("OnComboFinished"_h);
			}
		}

		TransitionTo(state, *targetMove, stamina);
		return true;
	}

	return false;
}

void MoveSystem::SetAttackHitboxesEnabled(Entity entity, World& world, const AttackParams& params, bool enabled)
{
	if (params.colliderNames.empty())
	{
		return;
	}

	CharacterHitboxes* hitboxes = world.GetComponent<CharacterHitboxes>(entity);

	if (!hitboxes)
	{
		return;
	}

	for (const auto& colliderName : params.colliderNames)
	{
		auto it = hitboxes->entities.find(HashString32(colliderName.c_str()));

		if (it == hitboxes->entities.end())
		{
			continue;
		}

		Collider* collider = world.GetComponent<Collider>(it->second);

		if (collider)
		{
			collider->isEnable = enabled;
		}
	}
}

void MoveSystem::ProcessAttack(Entity entity, World& world, MoveState& state, const MoveData& currentMove, float progress)
{
	const AttackParams& params = std::get<AttackParams>(currentMove.params);

	if (!state.canHit)
	{
		if (progress >= params.hitStartTime)
		{
			Attackable* attackable = world.GetComponent<Attackable>(entity);

			if (attackable)
			{
				attackable->entities.clear();
				attackable->isAttacking = true;
			}

			state.canHit = true;

			SetAttackHitboxesEnabled(entity, world, params, true);
		}

		return;
	}

	Attackable* attackable = world.GetComponent<Attackable>(entity);

	if (!attackable)
	{
		return;
	}

	if (progress < params.hitEndTime)
	{
		if (params.colliderNames.empty())
		{
			return;
		}

		CharacterHitboxes* hitboxes = world.GetComponent<CharacterHitboxes>(entity);

		if (!hitboxes)
		{
			return;
		}

		for (const auto& colliderName : params.colliderNames)
		{
			auto colliderIt = hitboxes->entities.find(HashString32(colliderName.c_str()));

			if (colliderIt == hitboxes->entities.end())
			{
				continue;
			}

			Collider* collider = world.GetComponent<Collider>(colliderIt->second);

			if (!collider)
			{
				continue;
			}

			if (collider->info.state != CollisionState::Enter &&
				collider->info.state != CollisionState::Stay)
			{
				continue;
			}

			Entity target = collider->info.other;

			auto alreadyHit = std::find(
				attackable->entities.begin(),
				attackable->entities.end(),
				target
			);

			if (alreadyHit != attackable->entities.end())
			{
				continue;
			}

			Damageable* damageable = world.GetComponent<Damageable>(target);

			if (!damageable)
			{
				continue;
			}

			Damage damage;
			damage.type = DamageType::Normal;
			damage.damage = params.damage;

			damageable->damageQueue.push(damage);
			attackable->entities.push_back(target);

			state.hitConfirm = true;
		}

		return;
	}

	attackable->isAttacking = false;
	SetAttackHitboxesEnabled(entity, world, params, false);
}

void MoveSystem::Update(World& world)
{
	View<MoveState, MoveInput, HP, Stamina> view(world);

	for (auto [entity, state, input, hp, stamina] : view)
	{
		if (hp.isDeath)
		{
			continue;
		}

		const CharacterInfo* charInfo = CharacterInfoRegistry::GetInstance()->GetCharacterInfo(state.name);

		if (!charInfo)
		{
			continue;
		}

		const MoveData& currentMove = CharacterInfoRegistry::GetInstance()->GetMoveById(
			state.name,
			state.currentMoveId
		);

		state.timer += Time::GetDeltaTime();

		float progress = 0.0f;

		if (currentMove.duration > 0.0f)
		{
			progress = state.timer / currentMove.duration;
		}

		if (currentMove.type == MoveType::Attack)
		{
			for (const auto& transition : currentMove.transitions)
			{
				if (transition.type != MoveTransitionType::InputPressed)
				{
					continue;
				}

				if (progress < transition.start || progress > transition.end)
				{
					continue;
				}

				// スタミナ不足の技はコンボ受付可能な遷移としても扱わない
				const MoveData* targetMove = GetTransitionTarget(state.name, transition, stamina);

				if (!targetMove)
				{
					continue;
				}

				AITrigger* trigger = world.GetComponent<AITrigger>(entity);

				if (trigger)
				{
					trigger->triggers.push_back("OnComboWindow"_h);
				}

				break;
			}
		}

		if (TryInputTransition(state, input, stamina, currentMove, progress))
		{
			continue;
		}

		if (currentMove.type == MoveType::Attack)
		{
			ProcessAttack(entity, world, state, currentMove, progress);
		}

		if (TryEndTransition(entity, world, state, input, stamina, currentMove, progress))
		{
			continue;
		}

		input.releasedKey = InputKey::None;
	}
}