#pragma once
#include "IComponentData.h"
#include "Vector3.h"

struct AABBCollider : IComponentData
{
    Vector3 offset = Vector3::zero;
    Vector3 size = Vector3(1.0f, 1.0f, 1.0f);
    bool isEnable = true;
    bool isTrigger = false;
    CollisionInfo info;
};