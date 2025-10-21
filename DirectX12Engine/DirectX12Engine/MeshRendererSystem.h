#pragma once

struct SceneConstants
{
    int activeLightCount;
    float padding[3];
    Vector4 cameraWorldPosition;
};

// オブジェクトごと（マテリアルごと）の定数
// HLSL側のcbufferと一致させ、256バイトアライメントを考慮したサイズにします
struct ObjectConstantsLayout
{
    Matrix4x4 worldMatrix;
    Color     diffuseColor;
    Color     specularColor; // 鏡面反射の色
    float     shininess;     // 光沢度
    float     obj_padding[3]; // パディング
};

// Material.hの TextureSlot::Max (ここでは3だが、多めに8とする)
static constexpr UINT MAX_SRV_TEXTURES_PER_OBJECT = 8;
// 1 (Materials: t0) + 8 (Textures: t1以降) = 9 スロット
static constexpr UINT MESH_DESCRIPTOR_SLOTS_PER_OBJECT = 1 + MAX_SRV_TEXTURES_PER_OBJECT;
static constexpr UINT MESH_TEXTURES_SLOT_OFFSET = 1; // t1 の開始位置はオフセット 1

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

    static inline ComPtr<Texture2D> m_defaultWhiteTexture;

    // DescriptorAllocatorの代わりに、フレーム全体で共有するGPU可視なデスクリプタヒープ
    static inline ComPtr<DescriptorHeap> m_frameDescriptorHeap;
    // 現在のフレームで次に利用可能なデスクリプタヒープの開始インデックス
    static inline UINT m_currentDescriptorIndex = 0;

    // リングバッファとして使用するオブジェクト定数バッファ
    static inline ComPtr<GraphicsBuffer> m_objectConstantBufferRing;
    // CPUから書き込むためのマップ済みポインタ
    static inline BYTE* m_mappedObjectConstants = nullptr;
    // 現在のフレームで描画したオブジェクトの数（リングバッファのインデックス）
    static inline UINT m_currentObjectBufferIndex = 0;

    // シーンごとの定数バッファ
    ComPtr<GraphicsBuffer> m_sceneConstantBuffer;

    std::unordered_map<Texture2D*, D3D12_GPU_DESCRIPTOR_HANDLE> m_srvCache;

public:
    static Texture2D* GetDefaultWhiteTexture() { return m_defaultWhiteTexture.Get(); }

public:
    /// <summary>
    /// レンダラーに描画するメッシュを設定します。
    /// </summary>
    void SetMesh(MeshRenderer* renderer, Mesh* mesh);

private:
    // 共有リソースを初期化・終了します
    static void StaticConstructor();
    static void StaticDestructor();

    D3D12_GPU_DESCRIPTOR_HANDLE GetSRV(Texture2D* tex, DescriptorAllocator* allocator);

    void Start(ComponentManager& cm, World& world) override;

    void Draw(ComponentManager& cm, World& world) override;
};