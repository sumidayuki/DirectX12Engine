#pragma once

/// <summary>
/// スプライトをレンダリングするための情報を持つコンポーネントです。
/// 定数バッファとディスクリプタヒープを作成する必要があります。
/// </summary>
struct SpriteRenderer
{
	bool					isEnabled = true;
	ComPtr<Sprite>			sprite = nullptr;
	Color					color = Color::white;
	bool					flipX = false;
	bool					flipY = false;
	ComPtr<GraphicsBuffer>	constantBuffer = nullptr;
	ComPtr<DescriptorHeap>	descriptorHeap = nullptr;

	bool					isStarted = false;
};