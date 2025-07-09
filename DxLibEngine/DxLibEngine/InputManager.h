#pragma once
#include "ButtonControl.h"
#include "KeyCode.h"
#include "Gamepad.h"


//----------------------------------------------------------------------------------------------------------------------------------------------------
// 入力デバイス統括クラス
//----------------------------------------------------------------------------------------------------------------------------------------------------
class InputManager
{
public:
    // 接続可能なゲームパッドの最大数
    static inline const int MaxGamepadCount = XUSER_MAX_COUNT;
    friend class Application;

private:
    // 通知先ウィンドウ
    static inline HWND m_hInputWnd;

    // ゲームパッド配列
    static inline Gamepad* m_gamepads[MaxGamepadCount];

private:
    // 入力デバイスサブシステムを初期化します。
    static void StaticConstructor(HWND hWnd);

    // 入力デバイスサブシステムを終了します。
    static void StaticDestructor();

    // 通知先ウィンドウを作成します。
    static HWND CreateInputWindow();

    // ウィンドウプロシージャ
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    // WM_INPUT時の処理
    static void OnInput(HRAWINPUT hRawInput);

    // マウス入力検知時の処理
    static void OnInputMouse(const RAWMOUSE& mouse);

    // キーボード入力検知時の処理
    static void OnInputKeyboard(const RAWKEYBOARD& keyboard);

    // 入力デバイスサブシステムを更新します。
    static void Update();

private:
    // キー数
    static constexpr int KeyCodeCount = 256;

    // キーボード/マウス/ジョイスティックのキー状態配列
    static inline bool m_keysLast[KeyCodeCount];
    static inline ButtonControl m_keys[KeyCodeCount];

public:
    // キーボード/マウス/ジョイスティックのキー状態を取得します。
    static const ButtonControl& GetKey(KeyCode key);


};


