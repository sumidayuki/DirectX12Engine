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
            // PlayerのTransformを取得
            m_playerTransform = world.GetComponent<Transform>(playerCamera.player);
            if (!m_playerTransform) continue; // プレイヤーが見つからない場合はスキップ
        }

        const Vector3 currentPosition = transform.position;

        const Vector2 mouseVelocity = Mouse::GetVelocity();
        m_yaw -= mouseVelocity.x * playerCamera.sensitivity;
        m_pitch = Mathf::Clamp(m_pitch + mouseVelocity.y * playerCamera.sensitivity, -40.0f, 80.0f);

        // 球面座標を使ってカメラの望ましい位置を計算
        const float distance = -playerCamera.offset.z;
        const float height = playerCamera.offset.y;
        const float yawRad = m_yaw * Mathf::Deg2Rad;
        const float pitchRad = m_pitch * Mathf::Deg2Rad;

        Vector3 offset;
        offset.x = distance * Mathf::Cos(pitchRad) * Mathf::Sin(yawRad);
        offset.y = distance * Mathf::Sin(pitchRad);
        offset.z = -distance * Mathf::Cos(pitchRad) * Mathf::Cos(yawRad);

        const Vector3 lookAtPoint = m_playerTransform->position + Vector3(0.0f, height, 0.0f);

        Vector3 targetPosition = lookAtPoint + offset;

        Ray ray = Ray(targetPosition, (lookAtPoint - targetPosition).Normalized());

        RaycastHit hit;

        if (Physics::Raycast(world, ray, hit, distance, Layers::Environment))
        {
            // 最終位置を計算
            targetPosition = hit.point;
        }

        TransformSystem::GetInstance()->SetLocalPosition(transform, targetPosition);

        // カメラを注視点に向ける (位置が決定した後で回転を適用)
        const Vector3 lookDirection = (lookAtPoint - transform.position).Normalized();
        TransformSystem::GetInstance()->SetLocalRotation(transform, Quaternion::LookRotation(lookDirection, Vector3::up));
    }
}
