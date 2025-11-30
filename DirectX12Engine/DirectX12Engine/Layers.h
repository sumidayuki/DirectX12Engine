#pragma once

// レイヤー定数の定義(32個まで)
namespace Layers
{
	// 既定のレイヤー
	constexpr LayerMask Default = (1 << 0);
	constexpr LayerMask TransparentFX = (1 << 1);
	constexpr LayerMask IgnoreRaycast = (1 << 2);

	constexpr LayerMask Water = (1 << 4);
	constexpr LayerMask UI = (1 << 5);

	// ユーザー用定義レイヤー
	constexpr LayerMask Environment = 1 << 8;

	// 全レイヤー
	constexpr LayerMask Everything = 0xFFFFFFFF;
}
