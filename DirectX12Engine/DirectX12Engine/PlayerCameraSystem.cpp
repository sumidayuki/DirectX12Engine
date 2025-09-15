#include "PlayerCameraSystem.h"
#include "PlayerCamera.h"

void PlayerCameraSystem::Start(ComponentManager& cm, World& world)
{
	m_yaw = 0.0f;
	m_pitch = 15.0f;
}

void PlayerCameraSystem::Draw(ComponentManager& cm, World& world)
{
	View<Transform, PlayerCamera> view(cm);

	for (auto [entity, transform, playerCamera] : view)
	{
		if (!m_playerTransform)
		{
			m_playerTransform = world.GetComponent<Transform>(*playerCamera.player);
		}

		const Vector2 mouseVelocity = Mouse::GetVelocity();
		m_yaw -= mouseVelocity.x * playerCamera.sensitivity;
		m_pitch = Mathf::Clamp(m_pitch + mouseVelocity.y * playerCamera.sensitivity, -40.0f, 80.0f);

		// カメラの水平回転に合わせてプレイヤーの向きを更新
		TransformSystem::SetLocalRotation(*m_playerTransform, Quaternion::Euler(0.0f, -m_yaw, 0.0f));

		// 球面座標を使ってカメラの位置を計算
		const float distance = -playerCamera.offset.z;
		const float height = playerCamera.offset.y;
		const float yawRad = m_yaw * Mathf::Deg2Rad;
		const float pitchRad = m_pitch * Mathf::Deg2Rad;

		Vector3 offset;
		offset.x = distance * Mathf::Cos(pitchRad) * Mathf::Sin(yawRad);
		offset.y = distance * Mathf::Sin(pitchRad);
		offset.z = -distance * Mathf::Cos(pitchRad) * Mathf::Cos(yawRad);

		const Vector3 lookAtPoint = m_playerTransform->position + Vector3(0.0f, height, 0.0f);
		TransformSystem::SetLocalPosition(transform, lookAtPoint + offset);

		// カメラを注視点に向ける
		const Vector3 lookDirection = (lookAtPoint - transform.position).Normalized();
		TransformSystem::SetLocalRotation(transform, Quaternion::LookRotation(lookDirection, Vector3::up));
	}
}
