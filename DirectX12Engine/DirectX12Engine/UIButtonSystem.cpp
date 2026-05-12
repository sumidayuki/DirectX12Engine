#include "UIButtonSystem.h"

void UIButtonSystem::Update(World& world)
{
    // イベントシステムの状態を取得
    EventSystem* eventState = nullptr;
    View<EventSystem> evtView(world);
    for (auto [e, evt] : evtView) 
    { 
        eventState = &evt; 
        break; 
    }

    if (!eventState) return;

    // 入力判定をキャッシュ
    bool mouseReleased = Mouse::GetButtonState(MouseButton::Left).WasReleasedThisFrame();
    bool controllerSubmit = InputManager::GetGamepad(0) && InputManager::GetGamepad(0)->A().WasPressedThisFrame();

    View<Button> btnView(world);
    for (auto [entity, button] : btnView)
    {
        button.wasClicked = false; // フラグのリセット

        UIGraphic* graphic = world.GetComponent<UIGraphic>(entity);
        if (!graphic) continue;

        UIButtonState nextState = UIButtonState::Normal;

        if (button.state == UIButtonState::Disabled)
        {
            nextState = UIButtonState::Disabled;
        }
        else if (eventState->currentPressed == entity)
        {
            nextState = UIButtonState::Pressed;
        }
        else if (eventState->currentHovered == entity)
        {
            nextState = UIButtonState::Hovered;
            // マウスを離したときにクリック成立
            if (button.state == UIButtonState::Pressed && mouseReleased) {
                button.wasClicked = true;
            }
        }
        else if (eventState->currentSelected == entity)
        {
            nextState = UIButtonState::Selected;
            // コントローラーのAボタン等でクリック成立
            if (controllerSubmit) {
                button.wasClicked = true;
            }
        }

        button.state = nextState;

        // 色の更新
        switch (nextState)
        {
            case UIButtonState::Normal:   
                graphic->color = button.normalColor;   
                break;

            case UIButtonState::Hovered:  
                graphic->color = button.hoveredColor;  
                break;

            case UIButtonState::Pressed:  
                graphic->color = button.pressedColor;  
                break;
            
            case UIButtonState::Selected: 
                graphic->color = button.selectedColor; 
                break;
        
            case UIButtonState::Disabled: 
                graphic->color = button.disabledColor; 
                break;
        }

        if (button.wasClicked && button.onClick)
        {
            button.onClick(); // 登録された関数を呼び出す
        }
    }
}