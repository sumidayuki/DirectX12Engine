#pragma once

namespace UIAPI
{
	// IMAGEコンポーネントにスプライトを設定します。
    void SetSprite(Image* uiImage, Sprite* sprite);

	// Canvasエンティティを作成します。
    Entity CreateCanvas(World& world, RenderMode mode = RenderMode::ScreenSpaceOverlay, int sortingOrder = 0);
	Entity CreateCanvas(World& world, const std::string& name, const Canvas& canvas);

	// Imageエンティティを作成します。
    Entity CreateImage(World& world, Entity canvasEntity, Sprite* sprite, Vector2 pos, Vector2 size, int depth = 0);
	Entity CreateImage(World& world, const std::string& name, Entity canvasEntity, const RectTransform& rect, const UIGraphic& graphic, const Image& image);
	 
	// Panelエンティティを作成します。
    Entity CreatePanel(World& world, Entity canvasEntity, Color color, Vector2 pos, Vector2 size, int depth = 0);
	Entity CreatePanel(World& world, const std::string& name, Entity canvasEntity, Color color, const RectTransform& rect, const UIGraphic& graphic);
	// Buttonエンティティを作成します。
    Entity CreateButton(World& world, Entity canvasEntity, Sprite* sprite, Vector2 pos, Vector2 size, int depth = 0);
	Entity CreateButton(World& world, const std::string& name, Entity canvasEntity, const RectTransform& rect, const UIGraphic& graphic, const Button& button);

	// Sliderエンティティを作成します。
    Entity CreateSlider(World& world, Entity canvasEntity, Vector2 pos, Vector2 size, Sprite* backgroundSprite = nullptr, Sprite* fillSprite = nullptr, float initialValue = 0.5f, int depth = 0);
	Entity CreateSlider(World& world, const std::string& name, Entity canvasEntity, const RectTransform& rect, const UIGraphic& graphic, const Slider& slider);
}