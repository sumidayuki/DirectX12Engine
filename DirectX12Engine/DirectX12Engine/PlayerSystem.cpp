#include "Precompiled.h"
#include "PlayerSystem.h"
#include "PlayerTag.h"
#include "PlayerCamera.h"

void PlayerSystem::Move(Transform& transform, Input& input, Animator& animator)
{
	// カメラのTransformがなければ処理しない
	if (!m_cameraTransform) return;

	// カメラの向きを基準とした前方と右方向を取得
	Vector3 camForward = m_cameraTransform->rotation * Vector3::forward;
	Vector3 camRight = m_cameraTransform->rotation * Vector3::right;

	// Y軸の傾きを無視して、水平なベクトルにする
	camForward.y = 0;
	camRight.y = 0;
	camForward.Normalized();
	camRight.Normalized();

	// カメラの向きと入力から、ワールド座標系での移動方向を決定
	Vector3 moveDirection = camForward * input.direction.y + camRight * input.direction.x;

	// アニメーションクリップの決定
	std::string targetClip = "Idle";

	// 移動入力があるかどうかの閾値
	const float moveThreshold = 0.01f;

	if (moveDirection.SqrMagnitude() > moveThreshold)
	{
		moveDirection = moveDirection.Normalized(); // 方向を正規化

		// 移動速度を決定
		m_currentSpeed = input.dash ? RunSpeed : WalkSpeed;

		// アニメーションを決定 (常に前進アニメーションを使用)
		targetClip = (m_currentSpeed == RunSpeed) ? "Run_Forward" : "Walk_Forward";

		// プレイヤーの向きを、移動方向に滑らかに向ける
		Quaternion targetRotation = Quaternion::LookRotation(moveDirection, Vector3::up);
		const float rotationSpeed = 15.0f; // 回転の速さ (値が大きいほど速い)
		transform.rotation = Quaternion::Slerp(transform.rotation, targetRotation, Time::GetDeltaTime() * rotationSpeed);

		// プレイヤーを移動させる
		TransformSystem::Translate(transform, moveDirection * m_currentSpeed * Time::GetDeltaTime());
	}

	// アニメーションの切り替え
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

	Vector3 pos = TransformSystem::GetPosition(*m_bowTransform);

	Entity* a = world.CreateWithModel(L"Assets/Arrow.fbx", nullptr, pos, Quaternion::LookRotation(forward));
}

void PlayerSystem::Start(ComponentManager& cm, World& world)
{
	// プレイヤーを生成
	Entity* player = world.CreateWithModel(L"Assets/player.fbx", nullptr, Vector3(0, 0, 0), Quaternion::identity);
	world.AddComponent<PlayerTag>(*player, PlayerTag{});
	world.AddComponent<Input>(*player, Input{});
	world.AddComponent<Velocity>(*player, Velocity{});

	// Playerの子エンティティから弓を発射する場所となる Transform を取得する。
	Transform* transform = world.GetComponent<Transform>(*player);
	m_bowTransform = TransformSystem::FindChild(transform, "mixamorig:LeftHandPinky4");

	// プレイヤーカメラの設定
	float fov = 60.0f;
	float aspect = (float)Screen::GetWidth() / (float)Screen::GetHeight();
	float nearPlane = 0.1f;
	float farPlane = 5000.0f;
	Vector3 cameraPos = Vector3(06.0f, 1.0f, -8.0f);
	
	// プレイヤーカメラの作成
	Entity* camera = world.CreateCamera3D(fov, aspect, nearPlane, farPlane, cameraPos);
	world.AddComponent<PlayerCamera>(*camera, PlayerCamera{ .player = player, .offset = Vector3(0, 150, -300), .sensitivity = 0.5f });

	m_cameraTransform = world.GetComponent<Transform>(*camera);

	// 初期値
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
