#pragma once

/// <summary>
/// スクリーン座標でUI画像を描画するためのコンポーネントです。
/// UnityのCanvasのように、ピクセル座標で位置とサイズを指定できます。
/// 座標系は左上(0,0)、右下(screenWidth, screenHeight)です。
/// </summary>
struct UIImage : IComponentData
{
	bool					isEnabled = true;

	/// <summary>
	/// スクリーン座標（ピクセル単位、左上原点）
	/// </summary>
	Vector2					position = Vector2::zero;

	/// <summary>
	/// 表示サイズ（ピクセル単位）
	/// </summary>
	Vector2					size = Vector2(100.0f, 100.0f);

	/// <summary>
	/// ピボット: (0,0)=左上, (0.5,0.5)=中央, (1,1)=右下
	/// </summary>
	Vector2					pivot = Vector2(0.0f, 0.0f);

	/// <summary>
	/// 色調
	/// </summary>
	Color					color = Color::white;

	/// <summary>
	/// テクスチャスプライト
	/// </summary>
	ComPtr<Sprite>			sprite = nullptr;

	/// <summary>
	/// 定数バッファ（UICanvasSystemが初期化時に作成）
	/// </summary>
	ComPtr<GraphicsBuffer>	constantBuffer = nullptr;

	/// <summary>
	/// SRVディスクリプタヒープ（UICanvasSystemが初期化時に作成）
	/// </summary>
	ComPtr<DescriptorHeap>	descriptorHeap = nullptr;

	/// <summary>
	/// 初期化済みフラグ
	/// </summary>
	bool					isStarted = false;
};
