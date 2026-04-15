#pragma once
#include "BehaviourTreeNode.h"

class BTCondition : public BTNode
{
    std::function<bool(Blackboard&)> m_func;
public:
    BTCondition(std::function<bool(Blackboard&)> func) : m_func(std::move(func)) {}
    BTStatus Tick(Blackboard& bb) override
    {
        return m_func(bb) ? BTStatus::Success : BTStatus::Failure;
    }
};
// アクションノード: ラムダで手軽にアクション定義
class BTAction : public BTNode
{
    std::function<BTStatus(Blackboard&)> m_func;
public:
    BTAction(std::function<BTStatus(Blackboard&)> func) : m_func(std::move(func)) {}
    BTStatus Tick(Blackboard& bb) override { return m_func(bb); }
};