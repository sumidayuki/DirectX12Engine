#pragma once
#include "BehaviourTreeNode.h"

struct BehaviourTree : IComponentData
{
	std::shared_ptr<BTNode> root;
	Blackboard blackboard;
};