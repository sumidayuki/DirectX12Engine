#pragma once

class ButtonControl
{
private:
	static inline const float DeltaTime = 1.0f / 60;

private:
	// リピートの進捗を表す列挙型
	enum class Progress
	{
		Released,			// 押されていない
		FirstPressed,		// 最初の検出
		FirstInterval,		// 最初の検出後の未検出時間中
		RepeatPressed,		// 2回目以降の検出
		RepeatInterval,		// 2回目以降の検出後の未検出時間中
	};

private:
	bool m_current;				// 現在のフレームで押されているか？
	bool m_previous;			// 1フレーム前からは押されていたか？	
	float m_firstInterval;		// リピートの最初の未検出時間（単位は秒）
	float m_repeatInterval;		// リピートの2回目以降の未検出時間（単位は秒）
	float m_elapsedTime;		// リピートの前回の検出からの経過時間（単位は秒）
	Progress m_progress;		// リピートの進捗状況

public:
	// コンストラクタ
	ButtonControl();

	// 入力状態を更新します。
	void Update(bool current);

	// 押されている場合は true を返します。
	bool IsPressed() const { return m_current; }

	// 現在のフレームで押された場合は true を返します。
	bool WasPressedThisFrame() const { return m_current && !m_previous; }

	// 現在のフレームで離された場合は true を返します。
	bool WasReleasedThisFrame() const { return !m_current && m_previous; }

	// リピート中の場合は true を返します。
	bool IsRepeated() const;
};