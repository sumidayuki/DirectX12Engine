#include "ProtoType.h"

void ProtoType::Instantiate(Entity e, ComponentManager& cm) const
{
    for (auto& add : m_addFuncs) 
    {
        add(e, cm);
    }
}
