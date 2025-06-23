#include "InputSystem.h"

void InputSystem::Update(ComponentManager& cm, World& world)
{
	View<Input> view(cm);

	// inputs��entity��input�ɕ����ĒT�����܂��B
	for (auto [entity, input] : view)
	{
		// �e�w��̃L�[�������ꂽ�牟����Ă��邱�Ƃ�����ɑΉ�����input�ɓ`���܂��B
		input.moveDown = Keyboard::GetKeyState(KeyCode::S).IsPressed();
		input.moveUp = Keyboard::GetKeyState(KeyCode::W).IsPressed();
		input.moveLeft = Keyboard::GetKeyState(KeyCode::A).IsPressed();
		input.moveRight = Keyboard::GetKeyState(KeyCode::D).IsPressed();
		input.shot = Keyboard::GetKeyState(KeyCode::Space).IsPressed();

		// moveDown ���� moveUp��������Ă��鎞 vartical �̒l��ύX���܂��B
		if (input.moveDown || input.moveUp)
		{
			if (input.moveDown)
			{
				input.vartical = 1;
			}

			if (input.moveUp)
			{
				input.vartical = -1;
			}
		}
		else
		{
			input.vartical = 0;
		}
		
		// moveRight ���� moveLeft��������Ă��鎞 horizontal �̒l��ύX���܂��B
		if (input.moveRight || input.moveLeft)
		{
			if (input.moveRight)
			{
				input.horizontal = 1;
			}

			if (input.moveLeft)
			{
				input.horizontal = -1;
			}
		}
		else
		{
			input.horizontal = 0;
		}
	}
}
