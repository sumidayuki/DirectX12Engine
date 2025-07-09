#pragma once
#include "ButtonControl.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------
// マウスボタンを表す列挙型
//----------------------------------------------------------------------------------------------------------------------------------------------------
enum class MouseButton
{
    Left,       // 左ボタン
    Right,      // 右ボタン
    Middle,     // 中央ボタン (チルトホイールの押し下げ)
    X1,         // 拡張ボタンその1
    X2,         // 拡張ボタンその2
};


//----------------------------------------------------------------------------------------------------------------------------------------------------
// マウスからの入力を扱う静的クラス
//----------------------------------------------------------------------------------------------------------------------------------------------------
class Mouse
{
    friend class InputManager;
    friend class Windows::NativeWindow;

private:
    // ウィンドウハンドル (「どのウィンドウ？」かを表す整数値)
    static inline HWND m_hWnd;

    // カーソルの動きを表す構造体
    struct CursorMotion
    {
        Vector2Int  position[2];    // 位置 ([0]:現在  [1]:前回)
        Vector2     velocity[2];    // 速度 ([0]:現在  [1]:前回)
        Vector2     acceleration;   // 加速度
    };

    // カーソルの動き
    static inline CursorMotion  m_motionInScreen;
    static inline CursorMotion  m_motionInClient;

    // ホイール速度
    static inline float m_wheelVelocity;

    // 各ボタンの状態
    static inline ButtonControl m_buttonStates[5];

private:
    // マウス入力システムを初期化します。
    static void StaticConstructor(HWND hWnd);

    // マウス入力システムを終了します。
    static void StaticDestructor();

    // マウスの入力情報を更新します。(毎フレーム呼び出そう！)
    static void Update();

    // ホイールが移動した際のイベント関数です。
    // この関数はウィンドウプロシージャから呼び出されます。
    // 
    // 奥側に1単位移動すると +1.0、
    // 手前側に1単位移動すると -1.0 のような値が渡されます。
    static void OnWheelMove(float wheelVelocity) { m_wheelVelocity = wheelVelocity; }

public:
    // マウスカーソルの位置を取得します。(スクリーン座標)
    static Vector2Int GetPositionInScreen() { return m_motionInScreen.position[0]; }

    // マウスカーソルの位置を取得します。(クライアント座標)
    static Vector2Int GetPosition() { return m_motionInClient.position[0]; }

    // マウスカーソルの速度を取得します。
    static Vector2 GetVelocity() { return m_motionInClient.velocity[0]; }

    // マウスカーソルの加速度を取得します。
    static Vector2 GetAcceleration() { return m_motionInClient.acceleration; }

    // ホイール速度を取得します。
    static float GetWheelVelocity() { return m_wheelVelocity; }

    // 指定したボタンの状態を取得します。
    static ButtonControl GetButtonState(MouseButton button);
};

