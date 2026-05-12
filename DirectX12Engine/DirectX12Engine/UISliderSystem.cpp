#include "UISliderSystem.h"

void UISliderSystem::Update(World& world)
{
    EventSystem* eventState = nullptr;
    View<EventSystem> evtView(world);
    for (auto [e, evt] : evtView)
    {
        eventState = &evt;
        break;
    }
    if (!eventState) return;

    Vector2Int mousePos = Mouse::GetPosition();
    float mx = (float)mousePos.x;
    float my = (float)mousePos.y;

    if (Windows::NativeWindow* win = Application::GetGameWindow())
    {
        Vector2Int clientSize = win->GetClientSize();
        if (clientSize.x > 0 && clientSize.y > 0)
        {
            mx = mx * (float)Screen::GetWidth() / (float)clientSize.x;
            my = my * (float)Screen::GetHeight() / (float)clientSize.y;
        }
    }

    bool mouseDown = Mouse::GetButtonState(MouseButton::Left).IsPressed();
    bool mousePressed = Mouse::GetButtonState(MouseButton::Left).WasPressedThisFrame();

    float controllerLeftX = InputManager::GetGamepad(0) ? InputManager::GetGamepad(0)->LeftStick().X().GetValue() : 0.0f;
    bool dpadLeft = InputManager::GetGamepad(0) && InputManager::GetGamepad(0)->DpadLeft().IsPressed();
    bool dpadRight = InputManager::GetGamepad(0) && InputManager::GetGamepad(0)->DpadRight().IsPressed();

    View<Slider> sliderView(world);
    for (auto [entity, slider] : sliderView)
    {
        RectTransform* rect = world.GetComponent<RectTransform>(entity);
        if (!rect) continue;

        bool isHovered = (eventState->currentHovered == entity);
        bool isSelected = (eventState->currentSelected == entity);

        if (isHovered && mousePressed)
        {
            slider.isDragging = true;
        }

        if (slider.isDragging)
        {
            if (mouseDown)
            {
                float t = (mx - rect->rectMin.x) / (rect->rectMax.x - rect->rectMin.x);
                t = (t < 0.0f) ? 0.0f : ((t > 1.0f) ? 1.0f : t);
                slider.value = slider.minValue + t * (slider.maxValue - slider.minValue);
            }
            else
            {
                slider.isDragging = false;
            }
        }

        if (isSelected)
        {
            float adjust = 0.0f;
            if (dpadLeft) adjust -= 0.01f;
            if (dpadRight) adjust += 0.01f;

            if (abs(controllerLeftX) > 0.2f)
            {
                adjust += controllerLeftX * 0.02f;
            }

            if (adjust != 0.0f)
            {
                slider.value += adjust * (slider.maxValue - slider.minValue);
                if (slider.value < slider.minValue) slider.value = slider.minValue;
                if (slider.value > slider.maxValue) slider.value = slider.maxValue;
            }
        }
    }
}
