#pragma once
#include "RectOffset.h"
#include "TextAnchor.h"

struct VerticalLayoutGroup : IComponentData
{
	float spacing = 0.0f;								// 子要素間のスペース
	RectOffset padding;									// パディング (子要素と親要素の間の余白)
	TextAnchor childAlignment = TextAnchor::UpperLeft;	// 子要素の配置方法
};