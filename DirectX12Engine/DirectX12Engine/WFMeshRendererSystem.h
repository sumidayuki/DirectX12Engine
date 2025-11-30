#pragma once

class WFMeshRendererSystem : public System
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

    void Start(World& world) override;

    void Draw(World& world) override;
};