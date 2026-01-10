#pragma once

enum class CollisionState
{
    None,
    Enter,
    Stay,
    Exit
};

struct CollisionInfo
{
    CollisionState state = CollisionState::None;
    Entity  other = INVALID_ENTITY; // Entity ID
};

enum class ColliderType
{
    Box,
    Sphere,
    AABB
};

struct Collider : IComponentData
{
    ColliderType type = ColliderType::Box;
    bool isEnable = true;
    bool isTrigger = false;
    Vector3 offset = Vector3::zero;
    Vector3 size = Vector3(1.0f, 1.0f, 1.0f);
    float radius = 0.5f;
    CollisionInfo info;
};
