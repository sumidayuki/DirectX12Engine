#pragma once
#include "AIDefinition.h"

class AIRuleImporter : public AssetImporter
{
public:
	void Import();

private:
	void ProcessRules(const Json& json);
	void ProcessTransitions(const Json& json, const std::string& stateName);
	void ProcessTransition(const Json& json, const std::string& stateName);
	void ProcessCondition(const Json& json, Transition& transition);
};