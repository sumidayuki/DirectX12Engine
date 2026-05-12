#include "UIAPI.h"

namespace UIAPI
{
	void SetSprite(Image* uiImage, Sprite* sprite)
	{
		uiImage->sprite = sprite;
	}

	Entity CreateCanvas(World& world, RenderMode mode, int sortingOrder)
	{
		Entity e = world.CreateEntity("Canvas");

		Canvas canvas;
		canvas.renderMode = mode;
		canvas.sortingOrder = sortingOrder;
		world.AddComponent<Canvas>(e, canvas);

		return e;
	}

	Entity CreateCanvas(World& world, const std::string& name, const Canvas& canvas)
	{
		Entity e = world.CreateEntity(name);
		world.AddComponent<Canvas>(e, canvas);
		return e;
	}

	static Entity CreateUIChild(World& world, Entity canvasEntity, const char* name, Vector2 pos, Vector2 size, Color color, int depth)
	{
		Entity e = world.CreateEntity(name);

		Transform* parentTransform = world.GetComponent<Transform>(canvasEntity);
		Transform* childTransform = world.GetComponent<Transform>(e);

		if (parentTransform && childTransform)
		{
			childTransform->parent = canvasEntity;

			if (parentTransform->firstChild.id == (EntitySize)-1)
			{
				parentTransform->firstChild = e;
			}
			else
			{
				Entity sibling = parentTransform->firstChild;
				Transform* siblingTrans = world.GetComponent<Transform>(sibling);
				while (siblingTrans && siblingTrans->nextSibling.id != (EntitySize)-1)
				{
					sibling = siblingTrans->nextSibling;
					siblingTrans = world.GetComponent<Transform>(sibling);
				}
				if (siblingTrans)
				{
					siblingTrans->nextSibling = e;
					childTransform->prevSibling = sibling;
				}
			}
		}

		RectTransform rect;
		rect.anchorMin = Vector2(0.0f, 0.0f);
		rect.anchorMax = Vector2(0.0f, 0.0f);
		rect.pivot = Vector2(0.0f, 0.0f);
		rect.sizeDelta = size;
		rect.anchoredPosition = pos;

		rect.rectMin = pos;
		rect.rectMax = pos + size;
		world.AddComponent<RectTransform>(e, rect);

		UIGraphic graphic;
		graphic.color = color;
		graphic.depth = depth;
		world.AddComponent<UIGraphic>(e, graphic);

		return e;
	}

	static Entity CreateUIChild(World& world, Entity canvasEntity, const char* name, const RectTransform& rect, const UIGraphic& graphic)
	{
		Entity e = world.CreateEntity(name);
		Transform* parentTransform = world.GetComponent<Transform>(canvasEntity);
		Transform* childTransform = world.GetComponent<Transform>(e);
		if (parentTransform && childTransform)
		{
			childTransform->parent = canvasEntity;
			if (parentTransform->firstChild.id == (EntitySize)-1)
			{
				parentTransform->firstChild = e;
			}
			else
			{
				Entity sibling = parentTransform->firstChild;
				Transform* siblingTrans = world.GetComponent<Transform>(sibling);
				while (siblingTrans && siblingTrans->nextSibling.id != (EntitySize)-1)
				{
					sibling = siblingTrans->nextSibling;
					siblingTrans = world.GetComponent<Transform>(sibling);
				}
				if (siblingTrans)
				{
					siblingTrans->nextSibling = e;
					childTransform->prevSibling = sibling;
				}
			}
		}
		world.AddComponent<RectTransform>(e, rect);
		world.AddComponent<UIGraphic>(e, graphic);
		return e;
	}

	Entity CreateImage(World& world, Entity canvasEntity, Sprite* sprite, Vector2 pos, Vector2 size, int depth)
	{
		Entity e = CreateUIChild(world, canvasEntity, "UIImage", pos, size, Color::white, depth);

		Image image;
		image.sprite = sprite;
		world.AddComponent<Image>(e, image);

		return e;
	}

	Entity CreateImage(World& world, const std::string& name, Entity canvasEntity, const RectTransform& rect, const UIGraphic& graphic, const Image& image)
	{
		Entity e = CreateUIChild(world, canvasEntity, name.c_str(), rect, graphic);
		world.AddComponent<Image>(e, image);
		return e;
	}

	Entity CreatePanel(World& world, Entity canvasEntity, Color color, Vector2 pos, Vector2 size, int depth)
	{
		Entity e = CreateUIChild(world, canvasEntity, "UIPanel", pos, size, color, depth);
		return e;
	}

	Entity CreatePanel(World& world, const std::string& name, Entity canvasEntity, Color color, const RectTransform& rect, const UIGraphic& graphic)
	{
		Entity e = CreateUIChild(world, canvasEntity, name.c_str(), rect, graphic);
		return e;
	}

	Entity CreateButton(World& world, Entity canvasEntity, Sprite* sprite, Vector2 pos, Vector2 size, int depth)
	{
		Entity e = CreateUIChild(world, canvasEntity, "UIButton", pos, size, Color::white, depth);

		if (sprite)
		{
			Image image;
			image.sprite = sprite;
			world.AddComponent<Image>(e, image);
		}

		Button button;
		world.AddComponent<Button>(e, button);

		return e;
	}

	Entity CreateButton(World& world, const std::string& name, Entity canvasEntity, const RectTransform& rect, const UIGraphic& graphic, const Button& button)
	{
		Entity e = CreateUIChild(world, canvasEntity, name.c_str(), rect, graphic);
		world.AddComponent<Button>(e, button);
		return e;
	}

	Entity CreateSlider(World& world, Entity canvasEntity, Vector2 pos, Vector2 size, Sprite* backgroundSprite, Sprite* fillSprite, float initialValue, int depth)
	{
		Entity e = CreateUIChild(world, canvasEntity, "UISlider", pos, size, Color::white, depth);

		Slider slider;
		slider.value = initialValue;
		slider.backgroundSprite = backgroundSprite;
		slider.fillSprite = fillSprite;
		world.AddComponent<Slider>(e, slider);

		return e;
	}

	Entity CreateSlider(World& world, const std::string& name, Entity canvasEntity, const RectTransform& rect, const UIGraphic& graphic, const Slider& slider)
	{
		Entity e = CreateUIChild(world, canvasEntity, name.c_str(), rect, graphic);
		world.AddComponent<Slider>(e, slider);
		return e;
	}
}