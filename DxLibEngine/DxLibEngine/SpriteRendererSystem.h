#pragma once

/// <summary>
/// 画像ファイルを描画するシステムです。
/// </summary>
class SpriteRendererSystem : public System
{
private:
    friend class Application;

private:
    // 全てのスプライトレンダラーで共有するオブジェクト
    static inline ComPtr<ID3D12PipelineState> m_graphicsPipelineState;
    static inline ComPtr<ID3D12RootSignature> m_rootSignature;

    // 共有リソースを初期化します。
    // (この関数はApplicationクラスからのみ呼び出されます)
    static void StaticConstructor();

    // 共有リソースの終了処理を行います。
    // (この関数はApplicationクラスからのみ呼び出されます)
    static void StaticDestructor();

	// 定数バッファのメモリレイアウトを表す構造体
	struct ConstantBufferLayout;

public:
    void SetSprite(SpriteRenderer* spriteRenderer, Sprite* sprite);

private:
	void Start(ComponentManager& cm, World& world) override;

	void Draw(ComponentManager& cm, World& world) override;
};