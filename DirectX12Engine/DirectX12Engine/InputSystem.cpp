#include "InputSystem.h"
#include "MoveInput.h"

void InputSystem::Update(World& world)
{
    View<Input, MoveInput> view(world);

    for (auto [entity, input, moveInput] : view)
    {
        const Gamepad* gamepad = InputManager::GetGamepad(0);

        // キーの状態を更新
        input.moveDown = !gamepad->IsConnected() ? Keyboard::GetKeyState(KeyCode::S).IsPressed() : gamepad->LeftStick().Down().IsPressed();
        input.moveUp = !gamepad->IsConnected() ? Keyboard::GetKeyState(KeyCode::W).IsPressed() : gamepad->LeftStick().Up().IsPressed();
        input.moveLeft = !gamepad->IsConnected() ? Keyboard::GetKeyState(KeyCode::A).IsPressed() : gamepad->LeftStick().Left().IsPressed();
        input.moveRight = !gamepad->IsConnected() ? Keyboard::GetKeyState(KeyCode::D).IsPressed() : gamepad->LeftStick().Right().IsPressed();

        //if (input.isGuard)
        //{
        //    input.guard2 = !gamepad->IsConnected() ? Keyboard::GetKeyState(KeyCode::LeftShift).WasReleasedThisFrame() : gamepad-//>GetButton(GamepadButton::LeftShoulder).WasReleasedThisFrame();
        //    input.isGuard = !input.guard2;
        //}
        //else
        //{
		//	input.guard1 = !gamepad->IsConnected() ? Keyboard::GetKeyState(KeyCode::LeftShift).WasPressedThisFrame() : gamepad-/>GetButton/(GamepadButton::LeftShoulder).WasPressedThisFrame();
        //    input.guard2 = false;
        //}

        const InputBind& inputBind = InputManager::GetInputBind();

		input.isGuard = !gamepad->IsConnected() ? Keyboard::GetKeyState(inputBind.GetKeyBindMap().at(InputKey::Guard)).IsPressed() : gamepad->GetButton(inputBind.GetButtonMap().at(InputKey::Guard)).IsPressed();

        input.dash = true;

		MouseButton mouseButton = inputBind.GetKeyBindMap().at(InputKey::Attack1) == KeyCode::Mouse0 ? MouseButton::Left : MouseButton::Right;

        input.attack1 = !gamepad->IsConnected() ? Mouse::GetButtonState(mouseButton).WasPressedThisFrame() : gamepad->GetButton(inputBind.GetButtonMap().at(InputKey::Attack1)).WasPressedThisFrame();
        input.attack2 = !gamepad->IsConnected() ? Mouse::GetButtonState(mouseButton).WasPressedThisFrame() : gamepad->GetButton(inputBind.GetButtonMap().at(InputKey::Attack2)).WasPressedThisFrame();

		input.isRolling = !gamepad->IsConnected() ? Keyboard::GetKeyState(inputBind.GetKeyBindMap().at(InputKey::Rolling)).WasPressedThisFrame() : gamepad->GetButton(inputBind.GetButtonMap().at(InputKey::Rolling)).WasPressedThisFrame();

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

        if (input.isRolling)
        {
            moveInput.inputKey = InputKey::Rolling;
			moveInput.timer = 0.0f;
        }

        if(input.isGuard)
        {
            moveInput.inputKey = InputKey::Guard;
            moveInput.timer = 0.0f;
		}

        if (input.attack1)
        {
            moveInput.inputKey = InputKey::Attack1;
            moveInput.timer = 0.0f;
        }

        moveInput.timer += Time::GetDeltaTime();
        if (moveInput.timer > 0.2f)
        {
            moveInput.inputKey = InputKey::None;
        }
    }
}