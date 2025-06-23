#include "MovementSystem.h"

void MovementSystem::Update(ComponentManager& cm, World& world)
{
    View<Transform, Velocity, Input> inputMove(cm);

    // inputsをentityとinputに分けて探索します。
    for (auto [entity, transform, velocity, input] : inputMove)
    {
        // 各種inputのフラグに適応した処理を実行します。
        if (input.moveUp)
        {
            transform.position.y -= velocity.speed;
        }
        if (input.moveDown)
        {
            transform.position.y += velocity.speed;
        }
        if (input.moveLeft)
        {
            transform.position.x -= velocity.speed;
        }
        if (input.moveRight)
        {
            transform.position.x += velocity.speed;
        }
    }

    View<Transform, Velocity, Target> targetMove(cm);

    // targetsをentityとtargetに分けて探索します。
    for (auto [entity, transform, velocity, target] : targetMove)
    {
        transform.position = cm.GetComponent<Transform>(target.entityID)->position;
    }
}
