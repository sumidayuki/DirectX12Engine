#pragma once

class AIAgentSystem : public System, public Singleton<AIAgentSystem>
{
	friend class Singleton<AIAgentSystem>;

public:
	void SetDestination(AIAgent& aiAgent, Vector3 target);
	void ResetAI(AIAgent& aiAgent);

private:
	void Update(ComponentManager& cm, World& world) override;
};