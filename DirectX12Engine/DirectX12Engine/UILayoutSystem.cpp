#include "Precompiled.h"
#include "UILayoutSystem.h"

void UILayoutSystem::Update(World& world)
{
    View<HorizontalLayoutGroup> hView(world);
    for (auto [entity, hGroup] : hView)
    {
        Transform* parentTransform = world.GetComponent<Transform>(entity);
        if (!parentTransform) continue;

        std::vector<RectTransform*> children;
        for (int i = 0; i < TransformSystem::GetInstance()->GetChildCount(parentTransform); i++)
        {
            Transform* childT = TransformSystem::GetInstance()->GetChild(parentTransform, i);

            UIGraphic* childUI = world.GetComponent<UIGraphic>(childT->entity);
            if (childUI && childUI->isEnabled)
            {
                RectTransform* childRT = world.GetComponent<RectTransform>(childT->entity);
                children.push_back(childRT);
            }
        }

        if (children.empty()) continue;

        float currentX = hGroup.padding.left;
        float centerYOffset = hGroup.padding.top;

        for (RectTransform* child : children)
        {
            float pivotXOffset = child->pivot.x * child->sizeDelta.x;
            child->anchoredPosition.x = currentX + pivotXOffset;

            currentX += child->sizeDelta.x + hGroup.spacing;
        }
    }

    View<VerticalLayoutGroup> vView(world);
    for (auto [entity, vGroup] : vView)
    {
        Transform* parentTransform = world.GetComponent<Transform>(entity);
        if (!parentTransform) continue;

        std::vector<RectTransform*> children;
        for (int i = 0; i < TransformSystem::GetInstance()->GetChildCount(parentTransform); i++)
        {
            Transform* childT = TransformSystem::GetInstance()->GetChild(parentTransform, i);
            
            UIGraphic* childUI = world.GetComponent<UIGraphic>(childT->entity);
            if (childUI && childUI->isEnabled)
            {
                RectTransform* childRT = world.GetComponent<RectTransform>(childT->entity);
                children.push_back(childRT);
            }
        }

        if (children.empty()) continue;

        float currentY = vGroup.padding.top;

        for (RectTransform* child : children)
        {
            float pivotYOffset = child->pivot.y * child->sizeDelta.y;
            child->anchoredPosition.y = currentY + pivotYOffset;

            currentY += child->sizeDelta.y + vGroup.spacing;
        }
    }
}
