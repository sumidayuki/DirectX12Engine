#include "InputSystem.h"

void InputSystem::Update(ComponentManager& cm, World& world)
{
    View<Input> view(cm);

    for (auto [entity, input] : view)
    {
        // キーの状態を更新
        input.moveDown = Keyboard::GetKeyState(KeyCode::S).IsPressed();
        input.moveUp = Keyboard::GetKeyState(KeyCode::W).IsPressed();
        input.moveLeft = Keyboard::GetKeyState(KeyCode::A).IsPressed();
        input.moveRight = Keyboard::GetKeyState(KeyCode::D).IsPressed();
        input.shot = Keyboard::GetKeyState(KeyCode::Space).IsPressed();
        input.dash = Keyboard::GetKeyState(KeyCode::LeftShift).IsPressed();
        input.attack = Mouse::GetButtonState(MouseButton::Left).WasPressedThisFrame();

        // 垂直方向の入力を計算
        // Wキーが押されている場合は1.0f、Sキーが押されている場合は-1.0f、両方またはどちらも押されていない場合は0.0f
        float vertical = 0.0f;
        if (input.moveUp && !input.moveDown) {
            vertical = 1.0f;
        }
        else if (!input.moveUp && input.moveDown) {
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
    }
}