#pragma once
#include "IComponentData.h"
#include "Vector3.h"

struct Rigidbody : public IComponentData
{
    Vector3 velocity = Vector3(0, 0, 0);
    Vector3 angularVelocity = Vector3(0, 0, 0);
    float mass = 1.0f;
    float drag = 0.0f;
    float angularDrag = 0.05f;
    bool useGravity = true;
    bool isKinematic = false;
};
