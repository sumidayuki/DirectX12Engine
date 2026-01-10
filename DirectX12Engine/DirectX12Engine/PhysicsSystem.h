#pragma once
#include "System.h"

class World;

class PhysicsSystem : public System
{
public:
    void Update(World& world) override;
};
