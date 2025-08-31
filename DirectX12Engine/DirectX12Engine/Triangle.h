#pragma once

/// <summary>
/// 三角形を表します。
/// 第一引数：左下の頂点(x, y)
/// 第二引数：右下の頂点(x, y)
/// 第三引数：天井の頂点(x, y)
/// 追加でRGBが設定できますが、デフォルトでは白色になっています。
/// </summary>
struct Triangle
{
	float x1, y1;
	float x2, y2;
	float x3, y3;
	int r = 255;
	int g = 255;
	int b = 255;

	int layer = 0;

	bool isActive = true;
};