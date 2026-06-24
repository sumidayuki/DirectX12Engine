#pragma once
#include "AIDefinition.h"

class AIRuleRegistry : public Singleton<AIRuleRegistry>
{
	friend class Singleton<AIRuleRegistry>;

private:
	std::unordered_map<std::uint32_t, std::unordered_map<std::uint32_t, std::vector<Transition>>> m_rules;

public:
	void RegisterRule(uint32_t characterId, uint32_t stateId, const Transition& transition);
	const std::vector<Transition>* GetTransitions(uint32_t characterId, uint32_t stateId) const;
};