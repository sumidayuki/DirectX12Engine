#pragma once

/// <summary>
/// エンティティのアニメーション状態を管理するコンポーネントです。
/// </summary>
struct Animator
{
	// このAnimatorが利用可能な全てのアニメーションクリップのマップ
	// キーはアニメーション名です。
	std::unordered_map<std::string, Animation*> clips;

	// 再生するアニメーションクリップ
	Animation* currentClip = nullptr;

	// メッシュに対応するスケルトン
	Skeleton* skeleton = nullptr;

	// 再生したいアニメーションクリップの名前
	// この名前を基にAnimationSystemがcurrentClipを切り替えます。
	std::string currentClipName;

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