#pragma once

enum RenderMode
{
	ScreenSpaceOverlay,		// スクリーンスペースオーバーレイ
	ScreenSpaceCamera,		// スクリーンスペースカメラ
	WorldSpace,				// ワールドスペース
};

struct UICanvas : IComponentData
{
	int sortingOrder = 0;									// 描画順序 (同じRenderMode内で、値が小さいほど先に描画されます)
	Vector3	position = Vector3::zero;						// キャンバスの位置 (スクリーンスペースの場合はピクセル座標、ワールドスペースの場合はワールド座標)
	Vector2	size = Vector2(1920.0f, 1080.0f);					// キャンバスのサイズ (スクリーンスペースの場合はピクセル単位、ワールドスペースの場合はワールド単位)
	RenderMode renderMode = RenderMode::ScreenSpaceOverlay;
};