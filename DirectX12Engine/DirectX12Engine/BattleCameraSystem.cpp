#include "BattleCameraSystem.h"
#include "BattleCamera.h"

void BattleCameraSystem::Start(World& world)
{
	m_currentCameraEntity = INVALID_ENTITY;
}

void BattleCameraSystem::Update(World& world)
{
    View<BattleCamera, Transform> view(world);

    for (auto [entity, battleCamera, transform] : view)
    {
        if (!battleCamera.isStarted)
        {
			m_characterTransform1 = world.GetComponent<Transform>(battleCamera.character1);
			m_characterTransform2 = world.GetComponent<Transform>(battleCamera.character2);

			battleCamera.isStarted = true;
        }

        m_currentCameraEntity = entity;

        if (!m_characterTransform1 || !m_characterTransform2) continue;

        // 注視点の計算（胸の高さ）
        float targetHeight = 120.0f;
        Vector3 pos1 = m_characterTransform1->position + Vector3(0, targetHeight, 0);
        Vector3 pos2 = m_characterTransform2->position + Vector3(0, targetHeight, 0);
        Vector3 midPoint = (pos1 + pos2) * 0.5f;

        // プレイヤー1から2への方向ベクトル
        Vector3 player1To2 = pos2 - pos1;
        float distance = player1To2.Magnitude();
        Vector3 forwardDir = player1To2.Normalized();

        // 「斜め後ろ」の位置を算出するための回転
        // プレイヤー1の真後ろから、少し横にずらす（例：30度）
        // クォータニオンを使用して方向ベクトルを回転させる
        float angle = 30.0f * 3.14159f / 180.0f; // 30度をラジアンに

        // Y軸周りに回転させて「斜め」の方向を作る
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);
        Vector3 diagonalBackDir;
        diagonalBackDir.x = -(forwardDir.x * cosA - forwardDir.z * sinA);
        diagonalBackDir.y = 0;
        diagonalBackDir.z = -(forwardDir.x * sinA + forwardDir.z * cosA);

        // ズーム距離の計算
        const float fovRad = 60.0f * 3.14159265f / 180.0f;
        float horizontalFovRad = 2.0f * std::atan(std::tan(fovRad * 0.5f) * battleCamera.aspect);
        float targetDistance = (distance * 0.5f) / std::tan(fovRad * 0.5f) * 1.5f;
		targetDistance = std::clamp(targetDistance, 200.0f, 400.0f);

        // 最終的な目標位置
        // プレイヤー1の位置を起点に、斜め後ろに引き、高さを出す
        Vector3 heightOffset = Vector3(0, 100.0f, 0);
        Vector3 desiredPosition = pos1 + (diagonalBackDir * targetDistance) + heightOffset;

        // 線形補間で滑らかに移動
        transform.position = Vector3::Lerp(transform.position, desiredPosition, 0.1f);

        // 中間点を向く
        transform.rotation = Quaternion::LookAt(transform.position, midPoint);

        transform.dirty = true;
    }
}