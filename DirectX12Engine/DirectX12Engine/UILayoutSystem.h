#pragma once

// System that overrides RectTransform positions for children
// of entities containing a HorizontalLayoutGroup or VerticalLayoutGroup.
// This should run before UICanvasSystem calculating final screen rects.
class UILayoutSystem : public System
{
public:
    void Update(World& world) override;
};
