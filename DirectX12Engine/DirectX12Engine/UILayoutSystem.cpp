#include "Precompiled.h"
#include "UILayoutSystem.h"

// Calculate total sizes and perform layout overrides
void UILayoutSystem::Update(World& world)
{
    // Horizontal Layouts
    View<HorizontalLayoutGroup> hView(world);
    for (auto [entity, hGroup] : hView)
    {
        Transform* parentTransform = world.GetComponent<Transform>(entity);
        if (!parentTransform) continue;

        // Collect valid RectTransform children
        std::vector<RectTransform*> children;
        Entity childEntity = parentTransform->firstChild;
        while (childEntity.id != (EntitySize)-1)
        {
            if (world.IsAlive(childEntity))
            {
                RectTransform* childRect = world.GetComponent<RectTransform>(childEntity);
                if (childRect) children.push_back(childRect);
            }
            Transform* cTrans = world.GetComponent<Transform>(childEntity);
            childEntity = cTrans ? cTrans->nextSibling : Entity{ (EntitySize)-1 };
        }

        if (children.empty()) continue;

        // Simple horizontal packing calculation
        float currentX = hGroup.padding.left;
        float centerYOffset = hGroup.padding.top; // Very basic alignment

        for (RectTransform* child : children)
        {
            // Override anchored position
            float pivotXOffset = child->pivot.x * child->sizeDelta.x;
            child->anchoredPosition.x = currentX + pivotXOffset;

            // Increment X
            currentX += child->sizeDelta.x + hGroup.spacing;
        }
    }

    // Vertical Layouts
    View<VerticalLayoutGroup> vView(world);
    for (auto [entity, vGroup] : vView)
    {
        Transform* parentTransform = world.GetComponent<Transform>(entity);
        if (!parentTransform) continue;

        std::vector<RectTransform*> children;
        Entity childEntity = parentTransform->firstChild;
        while (childEntity.id != (EntitySize)-1)
        {
            if (world.IsAlive(childEntity))
            {
                RectTransform* childRect = world.GetComponent<RectTransform>(childEntity);
                if (childRect) children.push_back(childRect);
            }
            Transform* cTrans = world.GetComponent<Transform>(childEntity);
            childEntity = cTrans ? cTrans->nextSibling : Entity{ (EntitySize)-1 };
        }

        if (children.empty()) continue;

        // Simple vertical packing
        float currentY = vGroup.padding.top;

        for (RectTransform* child : children)
        {
            float pivotYOffset = child->pivot.y * child->sizeDelta.y;
            child->anchoredPosition.y = currentY + pivotYOffset;

            // Increment Y
            currentY += child->sizeDelta.y + vGroup.spacing;
        }
    }
}
