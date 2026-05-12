#pragma once

enum RenderMode
{
	ScreenSpaceOverlay,		// スクリーンスペースオーバーレイ
	ScreenSpaceCamera,		// スクリーンスペースカメラ
	WorldSpace,				// ワールドスペース
};

struct Canvas : IComponentData
{
	RenderMode renderMode = RenderMode::ScreenSpaceOverlay;

	int sortingOrder = 0;									// 描画順序 (同じRenderMode内で、値が小さいほど先に描画されます)
};