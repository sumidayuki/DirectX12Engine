#include "MovementSystem.h"

void MovementSystem::Update(ComponentManager& cm, World& world)
{
    View<Transform, Velocity, Input> inputMove(cm);

    // inputs��entity��input�ɕ����ĒT�����܂��B
    for (auto [entity, transform, velocity, input] : inputMove)
    {
        // �e��input�̃t���O�ɓK���������������s���܂��B
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

    // targets��entity��target�ɕ����ĒT�����܂��B
    for (auto [entity, transform, velocity, target] : targetMove)
    {
        transform.position = cm.GetComponent<Transform>(target.entityID)->position;
    }
}
