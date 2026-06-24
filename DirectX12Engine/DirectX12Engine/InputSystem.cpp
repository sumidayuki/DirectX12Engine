#include "InputSystem.h"
#include "ComboInput.h"

void InputSystem::Update(World& world)
{
    View<Input, ComboInput> view(world);

    for (auto [entity, input, combo] : view)
    {
        const Gamepad* gamepad = InputManager::GetGamepad(0);

        // キーの状態を更新
        input.moveDown = !gamepad->IsConnected() ? Keyboard::GetKeyState(KeyCode::S).IsPressed() : gamepad->LeftStick().Down().IsPressed();
        input.moveUp = !gamepad->IsConnected() ? Keyboard::GetKeyState(KeyCode::W).IsPressed() : gamepad->LeftStick().Up().IsPressed();
        input.moveLeft = !gamepad->IsConnected() ? Keyboard::GetKeyState(KeyCode::A).IsPressed() : gamepad->LeftStick().Left().IsPressed();
        input.moveRight = !gamepad->IsConnected() ? Keyboard::GetKeyState(KeyCode::D).IsPressed() : gamepad->LeftStick().Right().IsPressed();

        if (combo.attackInputType != AttackInputType::Attack3)
        {
            input.guard1 = !gamepad->IsConnected() ? Keyboard::GetKeyState(KeyCode::LeftShift).WasPressedThisFrame() : gamepad->GetButton(GamepadButton::LeftShoulder).WasPressedThisFrame();
        }
        else
        {
            input.guard1 = false;
        }

        if (input.isGuard)
        {
            input.guard2 = !gamepad->IsConnected() ? Keyboard::GetKeyState(KeyCode::LeftShift).WasReleasedThisFrame() : gamepad->GetButton(GamepadButton::LeftShoulder).WasReleasedThisFrame();
            input.isGuard = !input.guard2;
        }
        else
        {
            input.guard2 = false;
        }

        input.dash = true;
        input.attack1 = !gamepad->IsConnected() ? Mouse::GetButtonState(MouseButton::Left).WasPressedThisFrame() : gamepad->GetButton(GamepadButton::RightShoulder).WasPressedThisFrame();
        input.attack2 = Mouse::GetButtonState(MouseButton::Right).WasPressedThisFrame();

        if (input.guard1)
        {
            input.isGuard = true;
        }

		input.isRolling = !gamepad->IsConnected() ? Keyboard::GetKeyState(KeyCode::Space).WasPressedThisFrame() : gamepad->GetButton(GamepadButton::LeftTrigger).WasPressedThisFrame();

        // 垂直方向の入力を計算
        // Wキーが押されている場合は1.0f、Sキーが押されている場合は-1.0f、両方またはどちらも押されていない場合は0.0f
        float vertical = 0.0f;
        if (input.moveUp && !input.moveDown) 
        {
            vertical = 1.0f;
        }
        else if (!input.moveUp && input.moveDown) 
        {
            vertical = -1.0f;
        }

        // 水平方向の入力を計算
        // Dキーが押されている場合は1.0f、Aキーが押されている場合は-1.0f、両方またはどちらも押されていない場合は0.0f
        float horizontal = 0.0f;
        if (input.moveRight && !input.moveLeft) {
            horizontal = 1.0f;
        }
        else if (!input.moveRight && input.moveLeft) {
            horizontal = -1.0f;
        }

        // directionベクトルを更新
        input.direction = Vector2(horizontal, vertical);

        // その他の入力は元のまま
        input.vartical = vertical;
        input.horizontal = horizontal;

        if (input.attack1)
        {
            combo.attackInputType = AttackInputType::Attack1;
            combo.timer = 0.0f;
        }

        if (input.isGuard && input.attack1)
        {
            input.isGuard = false;
            combo.attackInputType = AttackInputType::Attack3;
            combo.timer = 0.0f;
        }

        combo.timer += Time::GetDeltaTime();

        if (combo.timer > 0.2f)
        {
            combo.attackInputType = AttackInputType::Idle;
        }
    }
}