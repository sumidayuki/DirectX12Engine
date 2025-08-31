#pragma once

/// <summary>
/// エンティティのアニメーション状態を管理するコンポーネントです。
/// </summary>
struct Animator
{
	// 再生するアニメーションクリップ
	Animation* animation = nullptr;

	// 現在の再生時間
	float currentTime = 0.0f;

	// アニメーションが再生中かどうか
	bool isPlaying = true;

	// アニメーションをループさせるか
	bool isLoop = true;

	// ボーンの最終的な変換行列
	// AnimationSystemが毎フレーム計算し、SkinnedMeshRendererSystemが使用します。
	std::vector<Matrix4x4> finalBoneMatrices;
};