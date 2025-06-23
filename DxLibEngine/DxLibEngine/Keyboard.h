#pragma once

#include<DxLib.h>
#include "ButtonControl.h"
#include "KeyCode.h"

class Keyboard
{
private:
	// キーボードのキーの状態を格納する配列
	static inline ButtonControl m_keys[256];

public:
	// キーボードの入力情報を初期化します。
	static void Initialize();

	// キーボードの入力情報を更新します。（毎フレーム呼び出す）
	static void Update();

	// キーの訪台を取得します。
	static const ButtonControl& GetKeyState(KeyCode key);
};