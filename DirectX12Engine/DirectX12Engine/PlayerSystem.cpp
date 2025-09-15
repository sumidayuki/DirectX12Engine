#include "Precompiled.h"
#include "PlayerSystem.h"
#include "PlayerTag.h"
#include "PlayerCamera.h"

void PlayerSystem::Move(Transform& transform, Input& input, Animator& animator)
{
	Vector3 forward = transform.rotation * Vector3::forward;
	Vector3 right = transform.rotation * Vector3::right;

	forward.y = 0;
	right.y = 0;
	forward.Normalized();
	right.Normalized();

	// この行を修正
	Vector3 direction = forward * input.direction.y + right * input.direction.x;

	direction = Vector3::ClampMagnitude(direction, 0.0f, 1.0f);

	if (input.vartical > 0)
	{
		m_currentSpeed = input.dash ? RunSpeed : WalkSpeed;
	}
	else
	{
		m_currentSpeed = input.dash ? MiddleSpeed : WalkSpeed;
	}

	// 最終的な移動を適用
	// ここがあなたのコードから変更された最も重要な部分です
	TransformSystem::Translate(transform, direction * m_currentSpeed * Time::GetDeltaTime());

	std::string targetClip = "Idle";
	if (input.direction.y > 0.1f) // 前
	{
		targetClip = m_currentSpeed == WalkSpeed ? "Walk_Forward" : "Run_Forward";
	}
	else if (input.direction.y < -0.1f) // 後
	{
		targetClip = m_currentSpeed == WalkSpeed ? "Walk_Back" : "Run_Back";
	}
	else if (input.direction.x > 0.1f) // 右
	{
		targetClip = m_currentSpeed == WalkSpeed ? "Walk_Left" : "Run_Left";
	}
	else if (input.direction.x < -0.1f) // 左
	{
		targetClip = m_currentSpeed == WalkSpeed ? "Walk_Right" : "Run_Right";
	}

	if (animator.currentClipName != targetClip)
	{
		AnimationSystem::Play(animator, targetClip);
	}
}

void PlayerSystem::Attack(Transform& transform, World& world)
{
	Vector3 forward = transform.rotation * Vector3::forward;
	forward.y = 0;
	forward.Normalized();
	Entity* a = world.CreateWithModel(L"Assets/Arrow.fbx", nullptr, Vector3(transform.position.x, 30, transform.position.z), Quaternion::LookRotation(forward));
}

void PlayerSystem::Start(ComponentManager& cm, World& world)
{
	Entity* player = world.CreateWithModel(L"Assets/player.fbx", nullptr, Vector3(0, -100, 300), Quaternion::identity);

	world.AddComponent<PlayerTag>(*player, PlayerTag{});
	world.AddComponent<Input>(*player, Input{});
	world.AddComponent<Velocity>(*player, Velocity{});

	float fov = 60.0f;
	float aspect = (float)Screen::GetWidth() / (float)Screen::GetHeight();
	float nearPlane = 0.1f;
	float farPlane = 5000.0f;

	Vector3 cameraPos = Vector3(06.0f, 1.0f, -8.0f);
	Entity* camera = world.CreateCamera3D(fov, aspect, nearPlane, farPlane, cameraPos);
	world.AddComponent<PlayerCamera>(*camera, PlayerCamera{ .player = player, .offset = Vector3(0, 150, -300), .sensitivity = 0.5f });

	m_currentSpeed = WalkSpeed;
	m_stateTimer = 0.0f;
	m_currentState = PlayerState::Move;
}

void PlayerSystem::Update(ComponentManager& cm, World& world)
{
	View<PlayerTag, Transform, Input, Animator> view(cm);

	for (auto [entity, playerTag, transform, input, animator] : view)
	{
		switch (m_currentState)
		{
		case PlayerState::Move:
			m_stateTimer += Time::GetDeltaTime();

			Move(transform, input, animator);

			if (input.attack)
			{
				AnimationSystem::Play(animator, "Attack_01");
				m_currentState = PlayerState::Attack;
				animator.isLoop = false;
				m_stateTimer = 0;
			}
			break;

		case PlayerState::Attack:

			m_stateTimer += Time::GetDeltaTime();

			if (!animator.isPlaying)
			{
				Attack(transform, world);
				m_currentState = PlayerState::Move;
				animator.isLoop = true;
				m_stateTimer = 0;
			}

			break;
		}

	}
}
