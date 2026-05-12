#pragma once

struct UIGraphic : IComponentData
{
	bool isEnabled = true;

	Color color = Color::white;

	int depth = 0; // 描画順序 (描画される順番を制御するための深度値)

	bool raycastTarget = false; // レイキャストの対象かどうか (UI要素がユーザーの入力イベントを受け取るかどうか)
};