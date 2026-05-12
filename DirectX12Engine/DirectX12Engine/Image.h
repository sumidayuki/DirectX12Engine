#pragma once

/// <summary>
/// スクリーン座標でUI画像を描画するためのコンポーネントです。
/// UnityのCanvasのように、ピクセル座標で位置とサイズを指定できます。
/// 座標系は左上(0,0)、右下(screenWidth, screenHeight)です。
/// </summary>
struct Image : IComponentData
{
	ComPtr<Sprite>			sprite = nullptr;
};
