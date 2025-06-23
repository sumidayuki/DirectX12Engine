#include "InputSystem.h"

void InputSystem::Update(ComponentManager& cm, World& world)
{
	View<Input> view(cm);

	// inputsをentityとinputに分けて探索します。
	for (auto [entity, input] : view)
	{
		// 各指定のキーが押されたら押されていることをそれに対応するinputに伝えます。
		input.moveDown = Keyboard::GetKeyState(KeyCode::S).IsPressed();
		input.moveUp = Keyboard::GetKeyState(KeyCode::W).IsPressed();
		input.moveLeft = Keyboard::GetKeyState(KeyCode::A).IsPressed();
		input.moveRight = Keyboard::GetKeyState(KeyCode::D).IsPressed();
		input.shot = Keyboard::GetKeyState(KeyCode::Space).IsPressed();

		// moveDown 又は moveUpが押されている時 vartical の値を変更します。
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
		
		// moveRight 又は moveLeftが押されている時 horizontal の値を変更します。
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
