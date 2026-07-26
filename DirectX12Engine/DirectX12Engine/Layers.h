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
	constexpr LayerMask Player = 1 << 9;
	constexpr LayerMask Enemy = 1 << 10;

	// 全レイヤー
	constexpr LayerMask Everything = 0xFFFFFFFF;
}

inline std::map<std::string, LayerMask> StringToLayerMask = {
	{"Default", Layers::Default},
	{"TransparentFX", Layers::TransparentFX},
	{"IgnoreRaycast", Layers::IgnoreRaycast},
	{"Water", Layers::Water},
	{"UI", Layers::UI},
	{"Environment", Layers::Environment},
	{"Player", Layers::Player},
	{"Enemy", Layers::Enemy},
	{"Everything", Layers::Everything}
};
