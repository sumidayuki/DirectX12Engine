#pragma once

/// <summary>
/// 画像ファイルを描画するシステムです。
/// Spriteコンポーネントを所持している全てのエンティティのSprite情報を基に描画します。
/// </summary>
class SpriteRendererSystem : public System
{
public:
	void Draw(ComponentManager& cm, World& world) override;
};