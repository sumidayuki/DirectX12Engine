#include "AIRuleRegistry.h"

void AIRuleRegistry::RegisterRule(uint32_t characterId, uint32_t stateId, const Transition& transition)
{
  	m_rules[characterId][stateId].push_back(transition);
}

const std::vector<Transition>* AIRuleRegistry::GetTransitions(uint32_t characterId, uint32_t stateId) const
{
	auto characterIt = m_rules.find(characterId);
	if (characterIt != m_rules.end())
	{
		auto stateIt = characterIt->second.find(stateId);
		if (stateIt != characterIt->second.end())
		{
			return &stateIt->second;
		}
	}
	return nullptr;
}
