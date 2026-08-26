#pragma once

struct EffectSource : IComponentData
{
	// 再生するエフェクト
	Effect* effect = nullptr;

	// 開始時に自動再生するか
	bool playOnAwake = true;

	// 連続再生するか
	bool loop = false;

	// Transformに追従するか
	bool followTransform = true;
	
	// 再生時間
	float lifeTime = 1.0f;

	// 再生速度
	float speed = 1.0f;

	// カラー（元のエフェクトの色 * 指定した色）
	Color color = Color::white;

	// 再生状態
	bool isPlaying = false;
	bool isStarted = false;

	// APIからSystemへの要求
	bool requestPlay = false;
	bool requestStop = false;

	// Effekseer内部の再生ハンドル
	Effekseer::Handle handle = -1;
};