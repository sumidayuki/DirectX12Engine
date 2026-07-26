#include "Precompiled.h"
#include "AIStateMachineSystem.h"
#include "AIRuleRegistry.h"
#include "InputBind.h"

void AIStateMachineSystem::Update(World& world)
{
	View<AIState, CharacterStatus, MoveInput, AITrigger> view(world);

	for (auto [entity, aiState, characterStatus, moveInput, trigger] : view)
	{
		uint32_t aiFileName = FNV1a_Hash<uint32_t>(StatusAPI::GetString(characterStatus, "aiFileName").c_str());

		const std::vector<Transition>* currentTransitions = AIRuleRegistry::GetInstance()->GetTransitions(aiFileName, aiState.currentStateID);

		aiState.stateTime += Time::GetDeltaTime();

		if (currentTransitions)
		{
			for (const Transition& transition : *currentTransitions)
			{
				bool triggerMatched = false;

				// OnUpdate ‚ÍƒgƒŠƒK[‚Ì—L–³‚ÉŠÖ‚í‚ç‚¸í‚Éˆê’v‚Æ‚İ‚È‚·
				if (transition.trigger == "OnUpdate"_h)
				{
					triggerMatched = true;
				}
				else
				{
					for (const auto& t : trigger.triggers)
					{
						if (t == transition.trigger)
						{
							triggerMatched = true;
							break; // Œ©‚Â‚©‚Á‚½‚çŒŸõƒ‹[ƒv‚ğ”²‚¯‚é
						}
					}
				}

				if (triggerMatched == false)
				{
					continue; // ƒgƒŠƒK[‚ªˆê’v‚µ‚È‚¢ê‡‚ÍŸ‚Ì‘JˆÚ‚ğ•]‰¿
				}

				bool shouldTransition = true;
				for (const auto& condition : transition.conditions)
				{
					if (!condition(entity, world))
					{
						shouldTransition = false;
						break;
					}
				}

				if (shouldTransition)
				{
					if (aiState.currentStateID != transition.nextState)
					{
						aiState.stateTime = 0.0f;
					}

					aiState.currentStateID = transition.nextState;
					moveInput.inputKey = transition.input;
					break; // Å‰‚ÉğŒ‚ğ–‚½‚·‘JˆÚ‚ªŒ©‚Â‚©‚Á‚½‚çó‘Ô‘JˆÚ‚ğŠm’è‚³‚¹‚Äƒ‹[ƒv‚ğ”²‚¯‚é
				}
			}
		}

		trigger.triggers.clear();
	}
}