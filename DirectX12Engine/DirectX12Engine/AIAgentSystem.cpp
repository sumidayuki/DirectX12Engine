#include "Precompiled.h"
#include "AIAgentSystem.h"

void AIAgentSystem::SetDestination(AIAgent& aiAgent, Vector3 target)
{
	aiAgent.target = target;
}

void AIAgentSystem::ResetAI(AIAgent& aiAgent)
{
	aiAgent.target = Vector3::max;
}

void AIAgentSystem::Update(World& world)
{
    View<Transform, AIAgent> view(world);

    for (auto [entity, transform, aiAgent] : view)
    {
        // ターゲットが無効の値なら無視
        if (aiAgent.target == Vector3::max)
        {
            aiAgent.velocity.x = 0;
            aiAgent.velocity.z = 0;
            continue;
        }

        Vector3 toTarget = aiAgent.target - transform.position;
        Vector3 dirXZ = Vector3(toTarget.x, 0, toTarget.z);
        float distXZ = dirXZ.Magnitude();

        Vector3 targetVelXZ = Vector3::zero;
        if (distXZ > 0.05f)
        {
            targetVelXZ = dirXZ.Normalized() * aiAgent.speed;
        }
        else
        {
            // 到着した場合はターゲットをリセット
            aiAgent.target = Vector3::max;
        }

        // これにより、重力で落下中のY速度を消さずに、水平移動だけを制御できる
        float accel = aiAgent.acceleration * Time::GetDeltaTime();
        aiAgent.velocity.x = Mathf::Lerp(aiAgent.velocity.x, targetVelXZ.x, accel);
        aiAgent.velocity.z = Mathf::Lerp(aiAgent.velocity.z, targetVelXZ.z, accel);

        if (aiAgent.updatePosition)
        {
            TransformSystem::GetInstance()->Translate(transform, aiAgent.velocity * Time::GetDeltaTime());
        }

        if (aiAgent.updateRotation)
        {
            Vector3 lookDir = Vector3(aiAgent.velocity.x, 0, aiAgent.velocity.z);
            if (lookDir.SqrMagnitude() > 0.001f)
            {
                Quaternion targetRot = Quaternion::LookRotation(lookDir.Normalized());
                transform.rotation = Quaternion::Slerp(transform.rotation, targetRot, aiAgent.rotationSpeed * Time::GetDeltaTime());
            }
        }
    }
}