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
        Max
    };

private:
    Color m_diffuseColor;   // 拡散反射色
    Color m_specularColor;  // 鏡面反射色

    // テクスチャへのポインタ（参照カウントのため）
    ComPtr<Texture2D> m_textures[(int)TextureSlot::Max];

public:
    Material();
    ~Material() = default;

    void SetDiffuseColor(const Color& color) { m_diffuseColor = color; }
    const Color& GetDiffuseColor() const { return m_diffuseColor; }

    void SetSpecularColor(const Color& color) { m_specularColor = color; }
    const Color& GetSpecularColor() const { return m_specularColor; }

    /// <summary>
    /// 指定されたスロットにテクスチャを設定し、アロケータからデスクリプタを確保します。
    /// </summary>
    /// <param name="slot">テクスチャを設定するスロット</param>
    /// <param name="texture">設定するテクスチャ</param>
    /// <param name="allocator">SRVを確保するためのディスクリプタアロケータ</param>
    void SetTexture(TextureSlot slot, Texture2D* texture);

    Texture2D* GetTexture(TextureSlot slot) const { return m_textures[(int)slot].Get(); }
};