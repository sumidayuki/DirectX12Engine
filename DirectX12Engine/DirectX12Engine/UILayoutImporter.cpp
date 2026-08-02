#include "UILayoutImporter.h"
#include "ButtonFunctionRegistry.h"

static const std::unordered_map<std::string, TextAnchor> TextAnchorMap =
{
	{ "UpperLeft",		TextAnchor::UpperLeft },		// 上左
	{ "UpperCenter",	TextAnchor::UpperCenter },		// 上中央
	{ "UpperRight",		TextAnchor::UpperRight },		// 上右
	{ "MiddleLeft",		TextAnchor::MiddleLeft },		// 中左
	{ "MiddleCenter",	TextAnchor::MiddleCenter },		// 中央
	{ "MiddleRight",	TextAnchor::MiddleRight },		// 中右
	{ "LowerLeft",		TextAnchor::LowerLeft },		// 下左
	{ "LowerCenter",	TextAnchor::LowerCenter },		// 下中央
	{ "LowerRight",		TextAnchor::LowerRight }		// 下右
};


UILayoutImporter::UILayoutImporter()
{
}

bool UILayoutImporter::Import(const std::wstring path, World& world)
{
	SetAssetPath(path.c_str());
	return Import(world);
}

bool UILayoutImporter::Import(const wchar_t* path, World& world)
{
	SetAssetPath(path);
	return Import(world);
}

bool UILayoutImporter::Import(World& world)
{
	// パスを取得
	const std::wstring& path = GetAssetPath();
	
	// パスが空なら無効
	if (path.empty())
	{
		return false;
	}

	try
	{
		std::string text = StringUtility::ReadAllText(path);
		Json json = Json::parse(text);

		ProcessCanvas(json, world);
	}
	catch (const Json::parse_error& e)
	{
		std::ostringstream oss;
		oss << "JSON要素の解析中にエラーが発生: " << e.what() << "\n";
		OutputDebugStringA(oss.str().c_str());
	}
	catch (const Json::type_error& e)
	{
		std::ostringstream oss;
		oss << "JSON要素の解析中にエラーが発生: " << e.what() << "\n";
		OutputDebugStringA(oss.str().c_str());
	}
	catch (const Json::other_error& e)
	{
		std::ostringstream oss;
		oss << "JSON要素の解析中にエラーが発生: " << e.what() << "\n";
		OutputDebugStringA(oss.str().c_str());
	}
	catch (const Json::exception& e)
	{
		std::ostringstream oss;
		oss << "JSON要素の解析中にエラーが発生: " << e.what() << "\n";
		OutputDebugStringA(oss.str().c_str());
	}
	return true;
}

void UILayoutImporter::ProcessCanvas(const Json& json, World& world)
{
	const Json& canvasJson = json.value("canvas", Json::object());

	Canvas canvas;
	std::string renderMode = canvasJson.value("renderMode", "ScreenSpaceOverlay");
	if (renderMode == "ScreenSpaceOverlay")
	{
		canvas.renderMode = RenderMode::ScreenSpaceOverlay;
	}
	else if (renderMode == "ScreenSpaceCamera")
	{
		canvas.renderMode = RenderMode::ScreenSpaceCamera;
	}
	else if (renderMode == "WorldSpace")
	{
		canvas.renderMode = RenderMode::WorldSpace;
	}
	else
	{
		std::ostringstream oss;
		oss << "不明なRenderMode: " << renderMode << "\n";
		OutputDebugStringA(oss.str().c_str());
		canvas.renderMode = RenderMode::ScreenSpaceOverlay;
	}
	canvas.sortingOrder = canvasJson.value("sortingOrder", 0);

	Entity canvasEntity = UIAPI::CreateCanvas(world, UTF16LEtoUTF8::Convert(GetFileNameWithoutExtension()), canvas);
	UIManager::GetInstance()->AddCanvas(canvasEntity, HashString(UTF16LEtoUTF8::Convert(GetFileNameWithoutExtension()).c_str()));

	const Json& elements = canvasJson.value("elements", Json::array());
	for (const auto& elementJson : elements)
	{
		ProcessElement(elementJson, canvasEntity, world);
	}
}

void UILayoutImporter::ProcessElement(const Json& json, Entity parent, World& world)
{
	const std::string& type = json["type"].get<std::string>();

	if (type == "Image")
	{
		ProcessImage(json, parent, world);
	}
	else if (type == "Panel")
	{
		ProcessPanel(json, parent, world);
	}
	else if (type == "Button")
	{
		ProcessButton(json, parent, world);
	}
	else if (type == "Slider")
	{
		ProcessSlider(json, parent, world);
	}
	else if(type == "VerticalLayoutGroup")
	{
		ProcessVerticalLayout(json, parent, world);
	}
	else
	{
		std::ostringstream oss;
		oss << "不明なUI要素のタイプ: " << type << "\n";
		OutputDebugStringA(oss.str().c_str());
	}

	const Json& children = json.value("children", Json::array());
	for (const auto& childJson : children)
	{
		ProcessElement(childJson, parent, world);
	}
}

void UILayoutImporter::ProcessImage(const Json& json, Entity parent, World& world)
{
	const std::string& name = json.value("name", "UIImage");

	const Json& rectJson = json.value("rectT", Json::object());
	RectTransform rectT;
	ProcessRectTransform(rectJson, rectT);

	const Json& graphicJson = json.value("graphic", Json::object());
	UIGraphic graphic;
	ProcessUIGraphic(graphicJson, graphic);
	
	Texture2D* texture = AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, UTF8toUTF16LE::Convert(json.value("source", "")));

	std::vector<float> rectTemp = json.value("rect", std::vector<float>{ 0, 0, (float)texture->GetWidth(), (float)texture->GetHeight() });

	Rect rect;
	rect.x = rectTemp[0];
	rect.y = (float)texture->GetHeight() - rectTemp[1] - rectTemp[3];
	rect.width = rectTemp[2];
	rect.height = rectTemp[3];

	Sprite* sprite = Sprite::Create(texture, rect, Vector2(0.5f, 0.5f), 100.0f, 100.0f);

	Image image;
	image.sprite = sprite;

	Entity imageEntity = UIAPI::CreateImage(world, name, parent, rectT, graphic, image);
	UIManager::GetInstance()->AddUIObject(imageEntity, HashString(UTF16LEtoUTF8::Convert(GetFileNameWithoutExtension()).c_str()), HashString(name.c_str()));
}

void UILayoutImporter::ProcessPanel(const Json& json, Entity parent, World& world)
{
	const std::string& name = json.value("name", "UIPanel");

	const Json& rectJson = json.value("rect", Json::object());
	RectTransform rect;
	ProcessRectTransform(rectJson, rect);
	
	const Json& graphicJson = json.value("graphic", Json::object());
	UIGraphic graphic;
	ProcessUIGraphic(graphicJson, graphic);
	
	Entity panelEntity = UIAPI::CreatePanel(world, name, parent, Color::white, rect, graphic);
	UIManager::GetInstance()->AddUIObject(panelEntity, HashString(UTF16LEtoUTF8::Convert(GetFileNameWithoutExtension()).c_str()), HashString(name.c_str()));
}

void UILayoutImporter::ProcessButton(const Json& json, Entity parent, World& world)
{
	const std::string& name = json.value("name", "UIButton");

	const Json& rectJson = json.value("rect", Json::object());
	RectTransform rect;
	ProcessRectTransform(rectJson, rect);

	const Json& graphicJson = json.value("graphic", Json::object());
	UIGraphic graphic;
	ProcessUIGraphic(graphicJson, graphic);

	const std::string& sourcePath = json.value("source", "");
	Texture2D* texture = AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, UTF8toUTF16LE::Convert(sourcePath));
	Rect spriteRect = { 0, 0, (float)texture->GetWidth(), (float)texture->GetHeight() };
	Sprite* sprite = Sprite::Create(texture, spriteRect, Vector2(0.5f, 0.5f), 1.0f, 1.0f);

	std::vector<float> normalColor = json.value("normalColor", std::vector<float>{ 0.8f, 0.8f, 0.8f, 1.0f });
	std::vector<float> hoveredColor = json.value("hoveredColor", std::vector<float>{ 0.9f, 0.9f, 0.9f, 1.0f });
	std::vector<float> selectedColor = json.value("selectedColor", std::vector<float>{ 0.7f, 0.7f, 0.7f, 1.0f });
	std::vector<float> pressedColor = json.value("pressedColor", std::vector<float>{ 0.6f, 0.6f, 0.6f, 1.0f });
	std::vector<float> disabledColor = json.value("disabledColor", std::vector<float>{ 0.5f, 0.5f, 0.5f, 1.0f });

	Button button;
	button.sprite = sprite;
	button.onClick = ButtonFunctionRegistry::GetInstance()->GetFunction(json.value("onClick", ""));
	button.normalColor = Color(normalColor[0], normalColor[1], normalColor[2], normalColor[3]);
	button.hoveredColor = Color(hoveredColor[0], hoveredColor[1], hoveredColor[2], hoveredColor[3]);
	button.selectedColor = Color(selectedColor[0], selectedColor[1], selectedColor[2], selectedColor[3]);
	button.pressedColor = Color(pressedColor[0], pressedColor[1], pressedColor[2], pressedColor[3]);
	button.disabledColor = Color(disabledColor[0], disabledColor[1], disabledColor[2], disabledColor[3]);

	Entity buttonEntity = UIAPI::CreateButton(world, name, parent, rect, graphic, button);
	UIManager::GetInstance()->AddUIObject(buttonEntity, HashString(UTF16LEtoUTF8::Convert(GetFileNameWithoutExtension()).c_str()), HashString(name.c_str()));
}

void UILayoutImporter::ProcessSlider(const Json& json, Entity parent, World& world)
{
	const std::string& name = json.value("name", "UISlider");

	const Json& rectJson = json.value("rect", Json::object());
	RectTransform rect;
	ProcessRectTransform(rectJson, rect);

	const Json& graphicJson = json.value("graphic", Json::object());
	UIGraphic graphic;
	ProcessUIGraphic(graphicJson, graphic);

	const std::string& backgroundPath = json.value("background", "");
	std::vector<float> backgroundColor = json.value("backgroundColor", std::vector<float>{ 1.0f, 1.0f, 1.0f, 1.0f });
	Texture2D* backgroundTexture = AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, UTF8toUTF16LE::Convert(backgroundPath));
	Rect backgroundRect = { 0, 0, (float)backgroundTexture->GetWidth(), (float)backgroundTexture->GetHeight() };
	Sprite* backgroundSprite = Sprite::Create(backgroundTexture, backgroundRect, Vector2(0.5f, 0.5f), 1.0f, 1.0f);
	Color bgColor = Color(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);

	const std::string& fillPath = json.value("fill", "");
	std::vector<float> fillColor = json.value("fillColor", std::vector<float>{ 1.0f, 1.0f, 1.0f, 1.0f });
	Texture2D* fillTexture = AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, UTF8toUTF16LE::Convert(fillPath));
	Rect fillRect = { 0, 0, (float)fillTexture->GetWidth(), (float)fillTexture->GetHeight() };
	Sprite* fillSprite = Sprite::Create(fillTexture, fillRect, Vector2(0.5f, 0.5f), 1.0f, 1.0f);
	Color fColor = Color(fillColor[0], fillColor[1], fillColor[2], fillColor[3]);

	float initialValue = json.value("initialValue", 0.5f);

	Slider slider;
	slider.backgroundSprite = backgroundSprite;
	slider.backgroundColor = bgColor;
	slider.fillSprite = fillSprite;
	slider.fillColor = fColor;
	slider.value = initialValue;
	slider.isLeftToRight = json.value("isLeftToRight", true);

	Entity sliderEntity = UIAPI::CreateSlider(world, name, parent, rect, graphic, slider);
	UIManager::GetInstance()->AddUIObject(sliderEntity, HashString(UTF16LEtoUTF8::Convert(GetFileNameWithoutExtension()).c_str()), HashString(name.c_str()));
}

void UILayoutImporter::ProcessVerticalLayout(const Json& json, Entity parent, World& world)
{
	const std::string& name = json.value("name", "UIVerticalLayoutGroup");

	const Json& rectJson = json.value("rect", Json::object());
	RectTransform rect;
	ProcessRectTransform(rectJson, rect);

	float spacing = json.value("spacing", 0.0f);

	const Json& rectOffsetJson = json.value("rectOffset", Json::object());
	RectOffset rectOffset;
	ProcessRectOffset(rectOffsetJson, rectOffset);

	TextAnchor childAlignment = TextAnchor::UpperLeft;
	if (json.contains("textAnchor"))
	{
		std::string textAnchorStr = json.value("textAnchor", "UpperLeft");
		
		auto it = TextAnchorMap.find(textAnchorStr);
		if (it != TextAnchorMap.end())
		{
			childAlignment = it->second;
		}
	}
	
	Entity vlgEntity = world.CreateEntity(name, Layers::UI);
	
	VerticalLayoutGroup vlg;
	vlg.spacing = spacing;
	vlg.padding = rectOffset;
	vlg.childAlignment = childAlignment;

	world.AddComponent<RectTransform>(vlgEntity, rect);
	world.AddComponent<VerticalLayoutGroup>(vlgEntity, vlg);

	Transform* vlgT = world.GetComponent<Transform>(vlgEntity);
	Transform* parentT = world.GetComponent<Transform>(parent);
	TransformSystem::GetInstance()->SetParent(*vlgT, parentT);

	UIManager::GetInstance()->AddUIObject(vlgEntity, HashString(UTF16LEtoUTF8::Convert(GetFileNameWithoutExtension()).c_str()), HashString(name.c_str()));

	const Json& elementsJson = json.value("elements", Json::array());
	for (const auto& elementJson : elementsJson)
	{
		ProcessElement(elementJson, vlgEntity, world);
	}
}

void UILayoutImporter::ProcessRectTransform(const Json& json, RectTransform& rect)
{
	std::vector<float> anchoredPosition = json.value("pos", std::vector<float>{ 0.0f, 0.0f });
	std::vector<float> sizeDelta = json.value("size", std::vector<float>{ 100.0f, 100.0f });
	std::vector<float> anchorMin = json.value("anchorMin", std::vector<float>{ 0.5f, 0.5f });
	std::vector<float> anchorMax = json.value("anchorMax", std::vector<float>{ 0.5f, 0.5f });
	std::vector<float> pivot = json.value("pivot", std::vector<float>{ 0.5f, 0.5f });

	rect.anchoredPosition = Vector2(anchoredPosition[0], anchoredPosition[1]);
	rect.sizeDelta = Vector2(sizeDelta[0], sizeDelta[1]);
	rect.anchorMin = Vector2(anchorMin[0], anchorMin[1]);
	rect.anchorMax = Vector2(anchorMax[0], anchorMax[1]);
	rect.pivot = Vector2(pivot[0], pivot[1]);
}

void UILayoutImporter::ProcessRectOffset(const Json& json, RectOffset& rectOffset)
{
	float left = json.value("left", 0.0f);
	float right = json.value("right", 0.0f);
	float top = json.value("top", 0.0f);
	float bottom = json.value("bottom", 0.0f);

	rectOffset.left = left;
	rectOffset.right = right;
	rectOffset.top = top;
	rectOffset.bottom = bottom;
}

void UILayoutImporter::ProcessUIGraphic(const Json& json, UIGraphic& graphic)
{
	std::vector<float> color = json.value("color", std::vector<float>{ 1.0f, 1.0f, 1.0f, 1.0f });
	graphic.color = Color(color[0], color[1], color[2], color[3]);
	graphic.isEnabled = json.value("isEnabled", true);
	graphic.depth = json.value("depth", 0);
	graphic.raycastTarget = json.value("raycastTarget", false);
}
