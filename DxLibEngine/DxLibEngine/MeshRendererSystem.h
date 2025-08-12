#pragma once

struct SceneConstants
{
    int activeLightCount;
    float padding[3];
    Vector4 cameraWorldPosition;
};

// オブジェクトごと（マテリアルごと）の定数
struct ObjectConstantsLayout
{
    Matrix4x4 worldMatrix;
    Color     diffuseColor;
    Color     specularColor; // 鏡面反射の色
    float     shininess;     // 光沢度
    float     obj_padding[3]; // パディング
};

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

    static inline ComPtr<Texture2D> m_defaultWhiteTexture;

    // 定数バッファのレイアウト
    struct ConstantBufferLayout;

    ComPtr<GraphicsBuffer> m_sceneConstantBuffer;

public:
    static Texture2D* GetDefaultWhiteTexture() { return m_defaultWhiteTexture.Get(); }

public:
    /// <summary>
    /// レンダラーに描画するメッシュを設定します。
    /// </summary>
    void SetMesh(MeshRenderer* renderer, Mesh* mesh);

private:
    void Start(ComponentManager& cm, World& world) override;

    void Draw(ComponentManager& cm, World& world) override;
};