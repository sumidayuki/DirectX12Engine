#include "UIEventSystem.h"
#include "InputDeviceType.h"

void UIEventSystem::RaycastPointer(World& world, EventSystem* eventState)
{
	Vector2Int mousePos = Mouse::GetPosition();
	float mx = (float)mousePos.x;
	float my = (float)mousePos.y;

	if (Windows::NativeWindow* win = Application::GetGameWindow())
	{
		Vector2Int clientSize = win->GetClientSize();
		if(clientSize.x > 0 && clientSize.y > 0)
		{
			mx = mx * (float)Screen::GetWidth() / (float)clientSize.x;
			my = my * (float)Screen::GetHeight() / (float)clientSize.y;
		}
	}

	bool mouseDown = Mouse::GetButtonState(MouseButton::Left).IsPressed();

	struct HitRecord
	{
		Entity entity;
		int sortingOrder;
		int depth;
	};

	HitRecord baseHit{ { (EntitySize)-1 }, -999999, -999999 };

	View<Canvas> canvasView(world);

	for (auto [entity, canvas] : canvasView)
	{
		Transform* canvasTransform = world.GetComponent<Transform>(entity);

		if(canvas.renderMode == RenderMode::ScreenSpaceOverlay)
		{
			int childCount = TransformAPI::GetChildCount(canvasTransform);
			for(int i = 0; i < childCount; i++)
			{
				Entity childEntity = TransformAPI::GetChild(canvasTransform, i)->entity;
				
				if(!world.IsAlive(childEntity))
				{
					continue;
				}

				RectTransform* rect = world.GetComponent<RectTransform>(childEntity);
				UIGraphic* graphic = world.GetComponent<UIGraphic>(childEntity);

				if (rect && graphic && graphic->isEnabled && graphic->raycastTarget)
				{
					if(mx >= rect->rectMin.x && mx <= rect->rectMax.x && my >= rect->rectMin.y && my <= rect->rectMax.y)
					{
						if (graphic->depth > baseHit.depth || (graphic->depth == baseHit.depth && canvas.sortingOrder > baseHit.sortingOrder))
						{
							baseHit.entity = childEntity;
							baseHit.sortingOrder = canvas.sortingOrder;
							baseHit.depth = graphic->depth;
						}
					}
				}
			}
		}
	}

	eventState->currentHovered = baseHit.entity;

	if (mouseDown && baseHit.entity != Entity{ (EntitySize)-1 })
	{
		eventState->currentPressed = baseHit.entity;
	}
	else
	{
		eventState->currentPressed = Entity{ (EntitySize)-1 };
	}
}

void UIEventSystem::ProcessControllerNavigation(World& world, EventSystem* eventState)
{
	// 今後作る予定
}

void UIEventSystem::Start(World& world)
{
	EventSystem* eventState = nullptr;
	View<EventSystem> eventView(world);
	if (eventView.Empty())
	{
		Entity e = world.CreateEntity("EventSystem");
		world.AddComponent<EventSystem>(e, EventSystem{});
		eventState = world.GetComponent<EventSystem>(e);
	}
	else
	{
		for(auto [entity, es] : eventView)
		{
			if (eventState != nullptr)
			{
				world.DestroyEntity(entity);
				continue;
			}

			eventState = &es;
		}
	}
}

void UIEventSystem::Update(World& world)
{
	View<EventSystem> eventView(world);
	for(auto [entity, eventState] : eventView)
	{
		switch (InputManager::GetCurrentInputDeviceType())
		{
			case InputDeviceType::Keyboard_Mouse:
				// キーボードとマウスの入力処理
				RaycastPointer(world, &eventState);
				break;

			case InputDeviceType::Gamepad:
				// ゲームパッドの入力処理
				ProcessControllerNavigation(world, &eventState);
				break;

		default:
			break;
		}
	}
}
