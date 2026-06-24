#pragma once

class UILayoutImporter : public AssetImporter
{
public:
	UILayoutImporter();
	virtual ~UILayoutImporter() override = default;
	bool Import(const std::wstring path, World& world);
	bool Import(const wchar_t* path, World& world);
	bool Import(World& world);

private:
	void ProcessCanvas(const Json& json, World& world);
	void ProcessElement(const Json& json, Entity parent, World& world);
	void ProcessImage(const Json& json, Entity parent, World& world);
	void ProcessPanel(const Json& json, Entity parent, World& world);
	void ProcessButton(const Json& json, Entity parent, World& world);
	void ProcessSlider(const Json& json, Entity parent, World& world);
	void ProcessVerticalLayout(const Json& json, Entity parent, World& world);
	void ProcessRectTransform(const Json& json, RectTransform& rect);
	void ProcessRectOffset(const Json& json, RectOffset& rectOffset);
	void ProcessUIGraphic(const Json& json, UIGraphic& graphic);
};