#pragma once

/// <summary>
/// オブジェクトの表面の見た目を定義するアセット。
/// 色、テクスチャ、シェーダーのパラメータなどを管理します。
/// </summary>
class Material : public Reference
{
public:
    // シェーダー内のテクスチャスロットに対応させるためのインデックス
    enum class TextureSlot
    {
        Diffuse = 0,  // 基本色テクスチャ (t0)
        Normal,       // 法線マップ (t1)
        Specular,     // 鏡面反射マップ (t2)
        // 必要に応じて他のマップも追加
        Max
    };

private:
    // --- Material Properties ---
    Color m_diffuseColor;   // 拡散反射色
    Color m_specularColor;  // 鏡面反射色

    // --- Textures ---
    ComPtr<Texture2D> m_textures[(int)TextureSlot::Max];

    // --- DirectX 12 Resources ---
    ComPtr<DescriptorHeap> m_descriptorHeap;

public:
    Material();
    ~Material() = default;

    // --- Colors ---
    void SetDiffuseColor(const Color& color) { m_diffuseColor = color; }
    const Color& GetDiffuseColor() const { return m_diffuseColor; }

    void SetSpecularColor(const Color& color) { m_specularColor = color; }
    const Color& GetSpecularColor() const { return m_specularColor; }

    // --- Textures ---
    // C++の実装側で、設定された全テクスチャを元にデスクリプタヒープを再構築する必要がある
    void SetTexture(TextureSlot slot, Texture2D* texture);
    Texture2D* GetTexture(TextureSlot slot) const { return m_textures[(int)slot].Get(); }

    // --- DirectX 12 Getters ---
    ID3D12DescriptorHeap* GetDescriptorHeap() const { return m_descriptorHeap ? m_descriptorHeap->GetNativeHeapPointer() : nullptr; }

    // 特定スロットのデスクリプタハンドルを取得
    D3D12_GPU_DESCRIPTOR_HANDLE GetGpuDescriptorHandle(TextureSlot slot) const
    {
        return m_descriptorHeap ? m_descriptorHeap->GetGPUDescriptorHandle((int)slot) : D3D12_GPU_DESCRIPTOR_HANDLE{ 0 };
    }
};