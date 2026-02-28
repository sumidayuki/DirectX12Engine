#include "PlayerMoveSystem.h"

void PlayerMoveSystem::Update(World& world)
{
	View<Transform, Input, CharacterStatus, Animator, ComboState> view(world);

	for (auto [entity, transform, input, status, animator, state] : view)
	{
		if (state.currentMoveId != 0 || input.isGuard)
		{
			continue;
		}

		Transform* cameraTransform = world.GetComponent<Transform>(world.GetSystem<BattleCameraSystem>()->GetCurrentCameraEntity());

		// カメラのTransformがなければ処理しない
		if (!cameraTransform) return;

		// カメラの向きを基準とした前方と右方向を取得
		Vector3 camForward = cameraTransform->rotation * Vector3::forward;
		Vector3 camRight = cameraTransform->rotation * Vector3::right;

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

			// アニメーションを決定 (常に前進アニメーションを使用)
			targetClip = (input.dash) ? "Run_Forward" : "Walk_Forward";

			// プレイヤーの向きを、移動方向に滑らかに向ける
			Quaternion targetRotation = Quaternion::LookRotation(moveDirection, Vector3::up);
			const float rotationSpeed = 45.0f; // 回転の速さ (値が大きいほど速い)
			transform.rotation = Quaternion::Slerp(transform.rotation, targetRotation, Time::GetDeltaTime() * rotationSpeed);

			// プレイヤーを移動させる
			TransformSystem::GetInstance()->Translate(transform, moveDirection * (input.dash ? status.runSpeed : status.walkSpeed) * Time::GetDeltaTime());
		}

		// アニメーションの切り替え
		if (animator.currentClipName != targetClip)
		{
			AnimationSystem::Play(animator, targetClip);
		}
	}
}