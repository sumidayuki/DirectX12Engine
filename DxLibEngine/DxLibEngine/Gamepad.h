#pragma once
#include "InputControls.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------
// プレイヤーインデックス
//----------------------------------------------------------------------------------------------------------------------------------------------------
enum class PlayerIndex
{
    One,
    Two,
    Three,
    Four,
};


//----------------------------------------------------------------------------------------------------------------------------------------------------
// ゲームパッドボタン列挙型
//----------------------------------------------------------------------------------------------------------------------------------------------------
enum class GamepadButton
{
    South,                  // 下アクションボタン (「Aボタン」または「×ボタン」)
    East,                   // 右アクションボタン (「Bボタン」または「〇ボタン」)
    West,                   // 左アクションボタン (「Xボタン」または「□ボタン」)
    North,                  // 上アクションボタン (「Yボタン」または「△ボタン」)

    A = South,    // XBoxコントローラーの「Aボタン」
    B = East,     // XBoxコントローラーの「Bボタン」
    X = West,     // XBoxコントローラーの「Xボタン」
    Y = North,    // XBoxコントローラーの「Yボタン」

    Cross = South,    // PSコントローラーの「×ボタン」
    Circle = East,     // PSコントローラーの「〇ボタン」
    Square = West,     // PSコントローラーの「□ボタン」
    Triangle = North,    // PSコントローラーの「△ボタン」

    DpadDown,               // 十字キーの下
    DpadRight,              // 十字キーの右
    DpadLeft,               // 十字キーの左
    DpadUp,                 // 十字キーの上

    LeftShoulder,           // 左ショルダー           (「LBボタン」または「L1ボタン」)
    LeftTrigger,            // 左トリガー             (「LTボタン」または「L2ボタン」)
    LeftStickButton,        // 左スティックボタン     (「Lボタン」 または「L3ボタン」)
    RightShoulder,          // 右ショルダー           (「RBボタン」または「R1ボタン」)
    RightTrigger,           // 右トリガー             (「RTボタン」または「R2ボタン」)
    RightStickButton,       // 右スティックボタン     (「Rボタン」 または「R3ボタン」)

    Select,                 // セレクトボタン
    Start,                  // スタートボタン

    NumberOfButtons,        // 総ボタン数
};


//----------------------------------------------------------------------------------------------------------------------------------------------------
// ゲームパッドからの入力を扱うクラス
// 
//  ・ゲームパッドは複数個接続される可能性があるので非静的クラスが望ましい。
// 
//----------------------------------------------------------------------------------------------------------------------------------------------------
class Gamepad
{
public:
    // 総ボタン数
    static inline const int NumberOfButtons = (int)GamepadButton::NumberOfButtons;

    // 最後に使用された/接続されたゲームパッド
    static Gamepad* m_current;

private:
    // プレイヤーインデックス
    PlayerIndex m_playerIndex;

    // ボタンコントロール配列
    ButtonControl m_buttons[NumberOfButtons];

    // スティックコントロール
    StickControl m_leftStick;
    StickControl m_rightStick;

    bool m_isConnected;

public:
    // デフォルトコンストラクタ
    Gamepad(PlayerIndex playerIndex);

    // ゲームパッドの入力状態を更新します。
    void Update();

    // ゲームパッドが接続されている場合は true を返します。
    bool IsConnected() const { return m_isConnected; }

    // スティック
    const StickControl& LeftStick() const { return m_leftStick; }
    const StickControl& RightStick() const { return m_rightStick; }

    // 各種ボタン
    const ButtonControl& A() const { return m_buttons[(int)GamepadButton::A]; }
    const ButtonControl& B() const { return m_buttons[(int)GamepadButton::B]; }
    const ButtonControl& X() const { return m_buttons[(int)GamepadButton::X]; }
    const ButtonControl& Y() const { return m_buttons[(int)GamepadButton::Y]; }
    const ButtonControl& South() const { return m_buttons[(int)GamepadButton::South]; }
    const ButtonControl& East() const { return m_buttons[(int)GamepadButton::East]; }
    const ButtonControl& West() const { return m_buttons[(int)GamepadButton::West]; }
    const ButtonControl& North() const { return m_buttons[(int)GamepadButton::North]; }
    const ButtonControl& DpadDown() const { return m_buttons[(int)GamepadButton::DpadDown]; }
    const ButtonControl& DpadRight() const { return m_buttons[(int)GamepadButton::DpadRight]; }
    const ButtonControl& DpadLeft() const { return m_buttons[(int)GamepadButton::DpadLeft]; }
    const ButtonControl& DpadUp() const { return m_buttons[(int)GamepadButton::DpadUp]; }
    const ButtonControl& LeftShoulder() const { return m_buttons[(int)GamepadButton::LeftShoulder]; }
    const ButtonControl& LeftTrigger() const { return m_buttons[(int)GamepadButton::LeftTrigger]; }
    const ButtonControl& LeftStickButton() const { return m_buttons[(int)GamepadButton::LeftStickButton]; }
    const ButtonControl& RightShoulder() const { return m_buttons[(int)GamepadButton::RightShoulder]; }
    const ButtonControl& RightTrigger() const { return m_buttons[(int)GamepadButton::RightTrigger]; }
    const ButtonControl& RightStickButton() const { return m_buttons[(int)GamepadButton::RightStickButton]; }
    const ButtonControl& SelectButton() const { return m_buttons[(int)GamepadButton::Select]; }
    const ButtonControl& StartButton() const { return m_buttons[(int)GamepadButton::Start]; }

    // 指定したボタンの状態を取得します。
    const ButtonControl& GetButton(GamepadButton gamepadButton) const;
};
