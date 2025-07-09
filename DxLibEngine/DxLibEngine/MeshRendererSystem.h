#pragma once

/// <summary>
/// 3Dメッシュを描画するシステムです。
/// </summary>
class MeshRendererSystem : public System
{
private:
    friend class Application;

    // 共有リソース
    static inline ComPtr<ID3D12PipelineState> m_graphicsPipelineState;
    static inline ComPtr<ID3D12RootSignature> m_rootSignature;

    // 共有リソースを初期化・終了します
    // ※これらの関数は今後 Application クラスから呼び出す必要があります。
    static void StaticConstructor();
    static void StaticDestructor();

    // 定数バッファのレイアウト
    struct ConstantBufferLayout;

public:
    /// <summary>
    /// レンダラーに描画するメッシュを設定します。
    /// </summary>
    void SetMesh(MeshRenderer* renderer, Mesh* mesh);

private:
    void Draw(ComponentManager& cm, World& world) override;
};