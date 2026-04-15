#pragma once

/// <summary>
/// 3Dメッシュを描画するシステムです。
/// </summary>
class MeshRendererSystem : public System
{
private:
    friend class Application;

    // 定数バッファリソース (リングバッファ)
    static inline ComPtr<GraphicsBuffer> m_objectConstantBufferRing;
    // CPUから書き込むためのマップ済みポインタ
    static inline BYTE* m_mappedObjectConstants = nullptr;
    // 現在のフレームで描画したオブジェクトの数
    static inline UINT m_currentObjectBufferIndex = 0;

private:
    // 共有リソースを初期化・終了します
    static void StaticConstructor();
    static void StaticDestructor();

public:
    void Start(World& world) override;

    void Draw(World& world) override;
};