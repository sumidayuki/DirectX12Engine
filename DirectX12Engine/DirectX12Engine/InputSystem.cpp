#include "InputSystem.h"
#include "MoveInput.h"

void InputSystem::Update(World& world)
{
	View<Input, MoveInput> view(world);

	for (auto [entity, input, moveInput] : view)
	{
		InputDeviceType currentInputDeviceType = InputManager::GetCurrentInputDeviceType();

		const InputBind& inputBind = InputManager::GetInputBind();

		MouseButton mouseButton = inputBind.GetKeyBindMap().at(InputKey::Attack1) == KeyCode::Mouse0 ? MouseButton::Left : MouseButton::Right;

		bool guardPressed = false;
		bool guardReleased = false;

		// Releasedは1フレームのみ
		moveInput.releasedKey = InputKey::None;

		switch (currentInputDeviceType)
		{
			case InputDeviceType::Keyboard_Mouse:
			{
				input.moveDown = Keyboard::GetKeyState(KeyCode::S).IsPressed();
				input.moveUp = Keyboard::GetKeyState(KeyCode::W).IsPressed();
				input.moveLeft = Keyboard::GetKeyState(KeyCode::A).IsPressed();
				input.moveRight = Keyboard::GetKeyState(KeyCode::D).IsPressed();

				const auto guardState = Keyboard::GetKeyState(inputBind.GetKeyBindMap().at(InputKey::Guard));
				input.isGuard = guardState.IsPressed();
				guardPressed = guardState.WasPressedThisFrame();
				guardReleased = guardState.WasReleasedThisFrame();

				const MouseButton attack1Button =
					inputBind.GetKeyBindMap().at(InputKey::Attack1) == KeyCode::Mouse0
					? MouseButton::Left
					: MouseButton::Right;

				const MouseButton attack2Button =
					inputBind.GetKeyBindMap().at(InputKey::Attack2) == KeyCode::Mouse0
					? MouseButton::Left
					: MouseButton::Right;

				input.attack1 = Mouse::GetButtonState(attack1Button).WasPressedThisFrame();
				input.attack2 = Mouse::GetButtonState(attack2Button).WasPressedThisFrame();

				input.isRolling =Keyboard::GetKeyState(inputBind.GetKeyBindMap().at(InputKey::Rolling)).WasPressedThisFrame();

				break;
			}

			case InputDeviceType::Gamepad:
			{
				auto* gamepad = InputManager::GetGamepad(0);

				input.moveDown = gamepad->LeftStick().Down().IsPressed();
				input.moveUp = gamepad->LeftStick().Up().IsPressed();
				input.moveLeft = gamepad->LeftStick().Left().IsPressed();
				input.moveRight = gamepad->LeftStick().Right().IsPressed();

				const auto guardState = gamepad->GetButton(inputBind.GetButtonMap().at(InputKey::Guard));
				input.isGuard = guardState.IsPressed();
				guardPressed = guardState.WasPressedThisFrame();
				guardReleased = guardState.WasReleasedThisFrame();

				input.attack1 = gamepad->GetButton(inputBind.GetButtonMap().at(InputKey::Attack1)).WasPressedThisFrame();
				input.attack2 = gamepad->GetButton(inputBind.GetButtonMap().at(InputKey::Attack2)).WasPressedThisFrame();

				input.isRolling = gamepad->GetButton(inputBind.GetButtonMap().at(InputKey::Rolling)).WasPressedThisFrame();

				break;
			}

			default:
				break;
		}

		input.dash = true;

		// 移動入力
		float vertical = 0.0f;

		if (input.moveUp && !input.moveDown)
		{
			vertical = 1.0f;
		}
		else if (!input.moveUp && input.moveDown)
		{
			vertical = -1.0f;
		}

		float horizontal = 0.0f;

		if (input.moveRight && !input.moveLeft)
		{
			horizontal = 1.0f;
		}
		else if (!input.moveRight && input.moveLeft)
		{
			horizontal = -1.0f;
		}

		input.direction = Vector2(horizontal, vertical);

		input.vartical = vertical;

		input.horizontal = horizontal;

		// Move入力
		if (input.isRolling)
		{
			moveInput.inputKey = InputKey::Rolling;
			moveInput.timer = 0.0f;
		}

		if (guardPressed)
		{
			moveInput.inputKey = InputKey::Guard;
			moveInput.timer = 0.0f;
		}

		if (input.attack1)
		{
			moveInput.inputKey = InputKey::Attack1;
			moveInput.timer = 0.0f;
		}

		if (input.attack2)
		{
			moveInput.inputKey = InputKey::Attack2;
			moveInput.timer = 0.0f;
		}

		if (guardReleased)
		{
			moveInput.releasedKey = InputKey::Guard;
		}

		// 入力バッファ
		if (moveInput.inputKey != InputKey::None)
		{
			moveInput.timer += Time::GetDeltaTime();

			if (moveInput.timer > 0.2f)
			{
				moveInput.inputKey = InputKey::None;
				moveInput.timer = 0.0f;
			}
		}
	}
}