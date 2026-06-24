#pragma once

/// <summary>
/// オーディオを再生するためのコンポーネントです。
/// </summary>
struct AudioSource : IComponentData
{
	AudioClip* clip				= nullptr;	// 再生するオーディオクリップ

	float volume				= 1.0f;		// 音量 (0.0f - 1.0f)
	float pitch					= 1.0f;     // ピッチ (0.5f - 2.0f)
	bool loop					= false;    // ループ再生するか
	bool playOnAwake			= true;     // 起動時に自動で再生するか
	bool mute					= false;    // ミュートするか

	// 3D空間オーディオのパラメータ
	float spatialBlend			= 0.0f;		// 0=2D, 1=3D
	float minDistance			= 1.0f;		// 音が最大音量で聞こえる距離
	float maxDistance			= 500.0f;	// 音が聞こえなくなる距離
	
	// 内部状態 (AudioSystemによって管理される)
	bool isPlaying				= false;	// 現在再生中か
	bool requestPlay			= false;	// 再生要求フラグ
	bool requestStop			= false;	// 停止要求フラグ
	bool isStarted				= false;	// 再生が開始されたか

	IXAudio2SourceVoice* sourceVoice = nullptr; // XAudio2のソースボイス
};