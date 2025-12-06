#include "Precompiled.h"
#include "BoneSocketSystem.h"
#include "BoneSocket.h"

void BoneSocketSystem::Start(World& world)
{
}

void BoneSocketSystem::Update(World& world)
{
    View<BoneSocket, Transform> view(world);

    for (auto [entity, socket, transform] : view)
    {
        if (socket.targetEntity == INVALID_ENTITY) continue;

        // 対象のAnimatorを取得
        Animator* targetAnimator = world.GetComponent<Animator>(socket.targetEntity);
        if (!targetAnimator || !targetAnimator->skeleton) continue;

        // ボーンIDを検索（キャッシュ推奨だがここでは検索）
        const auto& boneInfoMap = targetAnimator->skeleton->GetBoneInfoMap();
        auto it = boneInfoMap.find(socket.targetBoneName);

        if (it != boneInfoMap.end())
        {
            int boneID = it->second.id;

            // AnimationSystemで計算済みの行列を取得！
            if (boneID < targetAnimator->socketGlobalMatrices.size())
            {
                Matrix4x4 boneGlobalTransform = targetAnimator->socketGlobalMatrices[boneID];

                // 親（キャラクター）のワールド行列を掛けてワールド座標にする
                Transform* parentTransform = world.GetComponent<Transform>(socket.targetEntity);
                Matrix4x4 parentWorldMatrix = parentTransform->localToWorldMatrix;

                // 最終的なワールド行列
                Matrix4x4 finalWorldMatrix = boneGlobalTransform * parentWorldMatrix;

                // Transformに適用（Decomposeして代入）
                Vector3 scale, pos;
                Quaternion rot;
                finalWorldMatrix.Decompose(scale, rot, pos);

                // オフセット適用（簡易実装）
                transform.position = pos + socket.posOffset;
                transform.rotation = rot * socket.rotOffset;
                transform.scale = scale; // 必要なら

                // 変更フラグを立てる
                transform.dirty = true;
                transform.hasChanged = true;
            }
        }
    }
}
