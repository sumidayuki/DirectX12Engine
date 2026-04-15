#pragma once
#include "BehaviourTreeNode.h"

struct BehaviourTree : IComponentData
{
	std::unique_ptr<BTNode> root;
	Blackboard blackboard;
};