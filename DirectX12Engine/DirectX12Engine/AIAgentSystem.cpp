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
		// ターゲットが無効の値なら無視する
		if (aiAgent.target == Vector3::max)
		{
			aiAgent.velocity = Vector3::zero;
			continue;
		}

		// ターゲットへの方向と距離
		Vector3 toTarget = aiAgent.target - transform.position;
		float distance = toTarget.Magnitude();

		// 到達判定
		if (distance < 0.05f)
		{
			aiAgent.velocity = Vector3::zero;
			aiAgent.target = Vector3::max;
			continue;
		}

		Vector3 dir = (aiAgent.target - transform.position).Normalized();
		Vector3 targetVelocity = dir * aiAgent.speed;

		aiAgent.velocity = Vector3::Lerp(aiAgent.velocity, targetVelocity, aiAgent.acceleration * Time::GetDeltaTime());

		if (aiAgent.updatePosition)
		{
			TransformSystem::GetInstance()->Translate(transform, aiAgent.velocity * Vector3(1, 0, 1) * Time::GetDeltaTime());
		}

		if (aiAgent.updateRotation)
		{
			Vector3 targetDir = aiAgent.velocity.Normalized();
			Quaternion targetRot = Quaternion::LookRotation(targetDir);
			transform.rotation = Quaternion::Slerp(transform.rotation, targetRot, aiAgent.rotationSpeed * Time::GetDeltaTime());
		}
	}
}
