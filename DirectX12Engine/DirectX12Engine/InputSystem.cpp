#include "InputSystem.h"

void InputSystem::Update(ComponentManager& cm, World& world)
{
    View<Input> view(cm);

    for (auto [entity, input] : view)
    {
        // �L�[�̏�Ԃ��X�V
        input.moveDown = Keyboard::GetKeyState(KeyCode::S).IsPressed();
        input.moveUp = Keyboard::GetKeyState(KeyCode::W).IsPressed();
        input.moveLeft = Keyboard::GetKeyState(KeyCode::A).IsPressed();
        input.moveRight = Keyboard::GetKeyState(KeyCode::D).IsPressed();
        input.shot = Keyboard::GetKeyState(KeyCode::Space).IsPressed();
        input.dash = Keyboard::GetKeyState(KeyCode::LeftShift).IsPressed();
        input.attack = Mouse::GetButtonState(MouseButton::Left).WasPressedThisFrame();

        // ���������̓��͂��v�Z
        // W�L�[��������Ă���ꍇ��1.0f�AS�L�[��������Ă���ꍇ��-1.0f�A�����܂��͂ǂ����������Ă��Ȃ��ꍇ��0.0f
        float vertical = 0.0f;
        if (input.moveUp && !input.moveDown) {
            vertical = 1.0f;
        }
        else if (!input.moveUp && input.moveDown) {
            vertical = -1.0f;
        }

        // ���������̓��͂��v�Z
        // D�L�[��������Ă���ꍇ��1.0f�AA�L�[��������Ă���ꍇ��-1.0f�A�����܂��͂ǂ����������Ă��Ȃ��ꍇ��0.0f
        float horizontal = 0.0f;
        if (input.moveRight && !input.moveLeft) {
            horizontal = 1.0f;
        }
        else if (!input.moveRight && input.moveLeft) {
            horizontal = -1.0f;
        }

        // direction�x�N�g�����X�V
        input.direction = Vector2(horizontal, vertical);

        // ���̑��̓��͂͌��̂܂�
        input.vartical = vertical;
        input.horizontal = horizontal;
    }
}