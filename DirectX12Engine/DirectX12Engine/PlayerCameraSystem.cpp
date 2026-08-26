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

        InputDeviceType deviceType = InputManager::GetCurrentInputDeviceType();

        playerCamera.currentScale = Mathf::Lerp(
            playerCamera.currentScale,
            playerCamera.targetScale,
            Time::GetDeltaTime() * playerCamera.zoomLerpFactor
        );

        Vector2 cameraInput = Vector2::zero;

        switch (deviceType)
        {
			// キーボードマウス
			case InputDeviceType::Keyboard_Mouse:
			{
			    cameraInput = Mouse::GetVelocity();

			    m_yaw -= cameraInput.x * playerCamera.sensitivity;
			    m_pitch = Mathf::Clamp(
			        m_pitch + cameraInput.y * playerCamera.sensitivity,
			        -40.0f,
			        80.0f
			    );
			    break;
			}

			// ゲームパッド
			case InputDeviceType::Gamepad:
			{
				const Gamepad* gamepad = InputManager::GetGamepad(0);

				if (gamepad && gamepad->IsConnected())
				{
					const StickControl& rightStick = gamepad->RightStick();

					const float x = rightStick.X().GetValue();
					const float y = -rightStick.Y().GetValue();

					constexpr float gamepadCameraSpeed = 120.0f;

					m_yaw -= x
						* gamepadCameraSpeed
						* Time::GetDeltaTime();

					m_pitch = Mathf::Clamp(
						m_pitch + y
						* gamepadCameraSpeed
						* Time::GetDeltaTime(),
						-40.0f,
						80.0f
					);
				}

				break;
			}
        }

        const float yawRad = m_yaw * Mathf::Deg2Rad;
        const float pitchRad = m_pitch * Mathf::Deg2Rad;

        const float currentDist =
            -playerCamera.offset.z * playerCamera.currentScale;
        const float currentHeight =
            playerCamera.offset.y * playerCamera.currentScale;
        const float currentSide =
            playerCamera.offset.x * playerCamera.currentScale;

        Vector3 cameraRight = Vector3(
            Mathf::Cos(yawRad),
            0,
            -Mathf::Sin(yawRad)
        );

        const Vector3 lookAtPoint =
            m_playerTransform->position
            + Vector3(0.0f, currentHeight, 0.0f)
            + (cameraRight * currentSide);

        Vector3 relativePos;
        relativePos.x =
            currentDist * Mathf::Cos(pitchRad) * Mathf::Sin(yawRad);
        relativePos.y =
            currentDist * Mathf::Sin(pitchRad);
        relativePos.z =
            -currentDist * Mathf::Cos(pitchRad) * Mathf::Cos(yawRad);

        Vector3 targetPosition = lookAtPoint + relativePos;

        Ray ray = Ray(lookAtPoint, relativePos.Normalized());
        RaycastHit hit;

        if (Physics::Raycast(
            world,
            ray,
            hit,
            currentDist,
            Layers::Environment))
        {
            targetPosition =
                hit.point
                + (lookAtPoint - targetPosition).Normalized() * 0.1f;
        }

        TransformAPI::SetLocalPosition(
            transform,
            targetPosition
        );

        const Vector3 lookDirection =
            (lookAtPoint - targetPosition).Normalized();

        TransformAPI::SetLocalRotation(
            transform,
            Quaternion::LookRotation(
                lookDirection,
                Vector3::up
            )
        );
    }
}