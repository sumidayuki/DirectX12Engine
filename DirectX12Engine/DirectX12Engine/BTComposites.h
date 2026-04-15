#pragma once

#include "BehaviourTreeNode.h"

class BTSelector : public BTNode
{
    std::vector<std::unique_ptr<BTNode>> m_children;
    int m_runningIndex = 0;

public:
    void AddChild(std::unique_ptr<BTNode> child) { m_children.push_back(std::move(child)); }
    
    BTStatus Tick(Blackboard& bb) override
    {
        for (int i = m_runningIndex; i < (int)m_children.size(); ++i)
        {
            BTStatus s = m_children[i]->Tick(bb);
            if (s == BTStatus::Running) { m_runningIndex = i; return BTStatus::Running; }
            if (s == BTStatus::Success) { m_runningIndex = 0; return BTStatus::Success; }
        }
        m_runningIndex = 0;
        return BTStatus::Failure;
    }
    
    void Reset() override { m_runningIndex = 0; for (auto& c : m_children) c->Reset(); }
};

// Sequence: 全ての子が Success なら Success（AND）
class BTSequence : public BTNode
{
    std::vector<std::unique_ptr<BTNode>> m_children;
    int m_runningIndex = 0;

public:
    void AddChild(std::unique_ptr<BTNode> child) { m_children.push_back(std::move(child)); }

    BTStatus Tick(Blackboard& bb) override
    {
        for (int i = m_runningIndex; i < (int)m_children.size(); ++i)
        {
            BTStatus s = m_children[i]->Tick(bb);
            if (s == BTStatus::Running) { m_runningIndex = i; return BTStatus::Running; }
            if (s == BTStatus::Failure) { m_runningIndex = 0; return BTStatus::Failure; }
        }
        m_runningIndex = 0;
        return BTStatus::Success;
    }

    void Reset() override { m_runningIndex = 0; for (auto& c : m_children) c->Reset(); }
};