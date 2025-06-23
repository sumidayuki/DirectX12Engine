#pragma once

enum class RenderType
{
	Sprite,
	Circle,
	Box,
	Triangle,
};

enum class Layer : int
{
	BackGround = 0,
	Bullet = 4,
	Enemy = 5,
	Player = 6,
	UI = 10,
	Default = 1,
};

/// <summary>
/// レンダリングを扱うコンポーネントです。
/// 第一成分：layer			（任意） 
/// 第二成分：orderInLayer	（任意） 
/// 第三成分：type			（レンダリングタイプを指定してください） 
/// 第四成分：sprite		（レンダリングタイプが Sprite の場合は設定してください） 
/// 第五成分：circle		（レンダリングタイプが Circle の場合は設定してください） 
/// 第六成分：box			（レンダリングタイプが Box の場合は設定してください） 
/// 第七成分：triangle		（レンダリングタイプが Triangle の場合は設定してください） 
/// </summary>
struct RenderCommand
{
	Layer layer = Layer::Default;
	int orderInLayer = 0;

	RenderType type;
	Sprite sprite = Sprite{};
	Circle circle = Circle{};
	Box box = Box{};
	Triangle triangle = Triangle{};
};