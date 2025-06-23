#pragma once

/// <summary>
/// 図形などの描画を行うシステムです。
/// 図形コンポーネントを所持している全てのエンティティの図形情報を基に描画します。
/// </summary>
class ShapeRendererSystem : public System
{
public:
	void Draw(ComponentManager& cm, World& world) override;
};