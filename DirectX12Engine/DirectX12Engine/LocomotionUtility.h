#pragma once

/// <summary>
/// ロコモーション方向計算のユーティリティ。
/// PlayerMoveSystem と EnemySystem の両方から使用する。
/// </summary>
namespace LocomotionUtility
{
    /// <summary>
    /// キャラクターの正面方向と移動方向の角度差を計算する（度数法、-180~+180）
    /// </summary>
    inline float CalculateMoveAngle(const Vector3& characterForward, const Vector3& moveDirection)
    {
        float dot = Vector3::Dot(characterForward, moveDirection);
        Vector3 cross = Vector3::Cross(characterForward, moveDirection);
        return Mathf::Atan2(cross.y, dot) * Mathf::Rad2Deg;
    }

    /// <summary>
    /// 角度差から4方向のアニメーションクリップ名を選択する。
    /// 各方向のクリップ名を引数で指定するため、プレイヤーと敵で異なるクリップ名を使用可能。
    /// </summary>
    inline std::string SelectDirectionalClip(float angle, const std::string& forwardClip,
        const std::string& backwardClip, const std::string& leftClip, const std::string& rightClip)
    {
        if (angle > -45.0f && angle <= 45.0f)
            return forwardClip;
        else if (angle > 45.0f && angle <= 135.0f)
            return rightClip;
        else if (angle > -135.0f && angle <= -45.0f)
            return leftClip;
        else
            return backwardClip;
    }
}
