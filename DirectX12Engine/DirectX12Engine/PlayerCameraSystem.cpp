#include "PlayerCameraSystem.h"
#include "PlayerCamera.h"
#include "DebugManager.h"

void PlayerCameraSystem::Start(World& world)
{
	m_yaw = 0.0f;
	m_pitch = 15.0f;
}

void PlayerCameraSystem::Draw(World& world)
{
    View<Transform, PlayerCamera> view(world);

    for (auto [entity, transform, playerCamera] : view)
    {
        if (!m_playerTransform)
        {
            m_playerTransform = world.GetComponent<Transform>(playerCamera.player);
            if (!m_playerTransform) continue;
        }

        playerCamera.currentScale = Mathf::Lerp(playerCamera.currentScale, playerCamera.targetScale, Time::GetDeltaTime() * playerCamera.zoomLerpFactor);

        const Vector2 mouseVelocity = Mouse::GetVelocity();
        m_yaw -= mouseVelocity.x * playerCamera.sensitivity;
        m_pitch = Mathf::Clamp(m_pitch + mouseVelocity.y * playerCamera.sensitivity, -40.0f, 80.0f);

        const float yawRad = m_yaw * Mathf::Deg2Rad;
        const float pitchRad = m_pitch * Mathf::Deg2Rad;

        // スケール適用
        const float currentDist = -playerCamera.offset.z * playerCamera.currentScale;
        const float currentHeight = playerCamera.offset.y * playerCamera.currentScale;
        const float currentSide = playerCamera.offset.x * playerCamera.currentScale;

        Vector3 cameraRight = Vector3(Mathf::Cos(yawRad), 0, -Mathf::Sin(yawRad));

        const Vector3 lookAtPoint = m_playerTransform->position
            + Vector3(0.0f, currentHeight, 0.0f)
            + (cameraRight * currentSide);

        // 球面座標から「注視点に対するカメラの相対位置」を計算
        Vector3 relativePos;
        relativePos.x = currentDist * Mathf::Cos(pitchRad) * Mathf::Sin(yawRad);
        relativePos.y = currentDist * Mathf::Sin(pitchRad);
        relativePos.z = -currentDist * Mathf::Cos(pitchRad) * Mathf::Cos(yawRad);

        // 最終的なターゲット位置
        Vector3 targetPosition = lookAtPoint + relativePos;

        Ray ray = Ray(lookAtPoint, relativePos.Normalized());
        RaycastHit hit;
        // 注視点からカメラ方向へ飛ばす
        if (Physics::Raycast(world, ray, hit, currentDist, Layers::Environment))
        {
            // 壁がある場合はヒット地点に寄せる
            targetPosition = hit.point + (lookAtPoint - targetPosition).Normalized() * 0.1f;
        }

        TransformSystem::GetInstance()->SetLocalPosition(transform, targetPosition);

        // 常に「ずらした後の注視点」を向く
        const Vector3 lookDirection = (lookAtPoint - targetPosition).Normalized();
        TransformSystem::GetInstance()->SetLocalRotation(transform, Quaternion::LookRotation(lookDirection, Vector3::up));
    }
}