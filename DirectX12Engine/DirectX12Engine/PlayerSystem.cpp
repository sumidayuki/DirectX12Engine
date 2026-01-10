#include "Precompiled.h"
#include "PlayerSystem.h"
#include "PlayerTag.h"
#include "PlayerCamera.h"
#include "ScopedProfiler.h"
#include "Arrow.h"

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
		const float rotationSpeed = 45.0f; // 回転の速さ (値が大きいほど速い)
		transform.rotation = Quaternion::Slerp(transform.rotation, targetRotation, Time::GetDeltaTime() * rotationSpeed);

		// プレイヤーを移動させる
		TransformSystem::GetInstance()->Translate(transform, moveDirection * m_currentSpeed * Time::GetDeltaTime());
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

	Vector3 pos = TransformSystem::GetInstance()->GetPosition(*m_bowTransform);

	Entity a = world.CreateWithModel(L"Assets/Arrow.fbx", nullptr, pos, Quaternion::LookRotation(forward));

	Projectile projectile;
	projectile.lifeTime = 1.0f;
	projectile.speed = 1500.0f;
	world.AddComponent<Projectile>(a, projectile);

	world.AddComponent<Attackable>(a, Attackable{});

	Collider collider;
	collider.type = ColliderType::Sphere;
	collider.radius = 10.0f;
	collider.isTrigger = true;
	world.AddComponent<Collider>(a, collider);

	world.AddComponent<Arrow>(a, Arrow{});
}

void PlayerSystem::Start(World& world)
{
	Entity camera = world.FindEntityOfType<PlayerCamera>();
	m_cameraTransform = world.GetComponent<Transform>(camera);

	// 初期値
	m_currentSpeed = WalkSpeed;
	m_stateTimer = 0.0f;
	m_currentState = PlayerState::Move;
}

void PlayerSystem::Update(World& world)
{
	View<PlayerTag, Transform, Input, Animator, AIAgent> view(world);

	for (auto [entity, playerTag, transform, input, animator, ai] : view)
	{
		if (!m_bowTransform)
		{
			m_bowTransform = TransformSystem::GetInstance()->FindChild(&transform, "mixamorig:RightHandPinky4");
		}

		switch (m_currentState)
		{
		case PlayerState::Move:
			m_stateTimer += Time::GetDeltaTime();

			Move(transform, input, animator);

			if (input.attack)
			{
				AnimationSystem::Play(animator, "Attack_00");
				AIAgentSystem::GetInstance()->SetDestination(ai, world.GetComponent<Transform>(world.GetSystem<GameManagerSystem>()->GetEnemy())->position);
				ai.updateRotation = true;
				m_currentState = PlayerState::Attack;
				animator.isLoop = false;
				m_stateTimer = 0;
			}
			break;

		case PlayerState::Attack:

			if (m_stateTimer == 0)
			{
				Attack(transform, world);
			}

			m_stateTimer += Time::GetDeltaTime();

			if (!animator.isPlaying)
			{
				ai.updateRotation = false;
				m_currentState = PlayerState::Move;
				animator.isLoop = true;
				m_stateTimer = 0;
			}

			break;
		}

	}
}
