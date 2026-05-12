#include "BehaviourTreeSystem.h"

void BehaviourTreeSystem::Update(World& world)
{
	View<BehaviourTree> view(world);
	for (auto [entity, bt] : view)
	{
        if (!bt.root) continue;
    
        // Blackboard を最新状態に更新
        bt.blackboard.world = &world;
        bt.blackboard.self = entity;
        
        // ツリーを1 Tick 実行
        bt.root->Tick(bt.blackboard);
    }
}
