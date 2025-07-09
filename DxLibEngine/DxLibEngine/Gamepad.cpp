#include "Gamepad.h"

Gamepad::Gamepad(PlayerIndex playerIndex)
    : m_playerIndex(playerIndex)
    , m_isConnected(false)
{

}


static float GetNormalizedValue(short value, unsigned short deadZone)
{
    // 入力値 value の絶対値がデッドゾーンの閾値を越えているか？
    if (value >= deadZone)
    {
        return value / 32767.0f;
    }
    else if (value <= -deadZone)
    {
        return value / 32768.0f;
    }
    else
    {
        return 0.0f;
    }
}


void Gamepad::Update()
{
    // i番目のゲームパッドの入力状態を読み取る
    XINPUT_STATE state;
    memset(&state, 0, sizeof(state));

    const DWORD result = XInputGetState((int)m_playerIndex, &state);
    if (result == ERROR_SUCCESS)
    {
        // ゲームパッドが接続されているので入力状態を更新
        m_isConnected = true;

        // ボタン
        m_buttons[(int)GamepadButton::A].Update((state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0);
        m_buttons[(int)GamepadButton::B].Update((state.Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0);
        m_buttons[(int)GamepadButton::X].Update((state.Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0);
        m_buttons[(int)GamepadButton::Y].Update((state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0);

        // 十字キー
        m_buttons[(int)GamepadButton::DpadUp].Update((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0);
        m_buttons[(int)GamepadButton::DpadLeft].Update((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0);
        m_buttons[(int)GamepadButton::DpadRight].Update((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0);
        m_buttons[(int)GamepadButton::DpadDown].Update((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0);

        // ショルダー
        m_buttons[(int)GamepadButton::LeftShoulder].Update((state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0);
        m_buttons[(int)GamepadButton::RightShoulder].Update((state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0);

        // 左トリガー
        const float leftTriggerValue = state.Gamepad.bLeftTrigger / 255.0f;
        m_buttons[(int)GamepadButton::LeftTrigger].Update(state.Gamepad.bLeftTrigger >= XINPUT_GAMEPAD_TRIGGER_THRESHOLD);

        // 右トリガー
        const float rightTriggerValue = state.Gamepad.bRightTrigger / 255.0f;
        m_buttons[(int)GamepadButton::RightTrigger].Update(state.Gamepad.bRightTrigger >= XINPUT_GAMEPAD_TRIGGER_THRESHOLD);

        // スティックボタン
        m_buttons[(int)GamepadButton::LeftStickButton].Update((state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != 0);
        m_buttons[(int)GamepadButton::RightStickButton].Update((state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0);

        // スタート/セレクト
        m_buttons[(int)GamepadButton::Start].Update((state.Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0);
        m_buttons[(int)GamepadButton::Select].Update((state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != 0);

        // 左スティック
        Vector2 leftStickValue;
        leftStickValue.x = GetNormalizedValue(state.Gamepad.sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
        leftStickValue.y = GetNormalizedValue(state.Gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
        m_leftStick.Update(leftStickValue);

        // 右スティック
        Vector2 rightStickValue;
        rightStickValue.x = GetNormalizedValue(state.Gamepad.sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
        rightStickValue.y = GetNormalizedValue(state.Gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
        m_rightStick.Update(rightStickValue);
    }
    else if (result == ERROR_DEVICE_NOT_CONNECTED)
    {
        // ゲームパッドが接続されていないので入力状態をクリア
        m_isConnected = false;

        for (int i = 0; i < NumberOfButtons; i++)
        {
            m_buttons[i].Update(false);
        }

        m_leftStick.Update(Vector2::zero);
        m_rightStick.Update(Vector2::zero);
    }
    else
    {
        // 不明なエラー
        assert(0);
    }
}


const ButtonControl& Gamepad::GetButton(GamepadButton gamepadButton) const
{
    return m_buttons[(int)gamepadButton];
}

