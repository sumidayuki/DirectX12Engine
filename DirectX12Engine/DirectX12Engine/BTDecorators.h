#pragma once
#include "BehaviourTreeNode.h"

class BTInverter : public BTNode
{
private:
    std::unique_ptr<BTNode> m_child;

public:
    BTInverter(std::unique_ptr<BTNode> child) : m_child(std::move(child)) {}
    
    BTStatus Tick(Blackboard& bb) override
    {
        BTStatus s = m_child->Tick(bb);
        if (s == BTStatus::Success) return BTStatus::Failure;
        if (s == BTStatus::Failure) return BTStatus::Success;
        return BTStatus::Running;
    }
};
// Repeater: Žq‚ð N ‰ñŒJ‚è•Ô‚·
class BTRepeater : public BTNode
{
private:
    std::unique_ptr<BTNode> m_child;
    int m_count, m_current = 0;

public:
    BTRepeater(std::unique_ptr<BTNode> child, int count)
        : m_child(std::move(child)), m_count(count) {
    }

    BTStatus Tick(Blackboard& bb) override
    {
        while (m_current < m_count)
        {
            BTStatus s = m_child->Tick(bb);
            if (s == BTStatus::Running) return BTStatus::Running;
            m_current++;
        }
        m_current = 0;
        return BTStatus::Success;
    }
};