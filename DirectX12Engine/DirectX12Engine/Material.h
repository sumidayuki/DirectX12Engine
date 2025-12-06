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
        Diffuse = 0,        // 基本色テクスチャ      (t0)
        //Normal,             // 法線マップ            (t1)
        //MetallicRoughness,  // 金属度と粗さマップ    (t2)
        //Occlusion,          // 環境遮蔽マップ        (t3)
        //Emissive,           // 自発光カラー          (t4)
        Max
    };

    // マテリアルが使用するシェーダ機能フラグ
    struct ShaderFlags
    {
        bool HasNormalMap               = false;
        bool HasMatellicRoughnessMap    = false;
        bool IsAlphaTested              = false;
    };

private:
    Color m_baseColor;
    float m_roughness;
    float m_metallic;
    Color m_emissiveColor;

    ShaderFlags m_shaderFlags;

    // テクスチャへのポインタ（参照カウントのため）
    ComPtr<Texture2D> m_textures[(int)TextureSlot::Max];

public:
    Material();
    ~Material() = default;

    void SetBaseColor(const Color& color) { m_baseColor = color; }
    void SetRoughness(float r) { m_roughness = r; }
    void SetMetallic(float m) { m_metallic = m; }
    void SetEmissiveColor(const Color& color) { m_emissiveColor = color; }

    const Color& GetBaseColor() const { return m_baseColor; }
    const float& GetRoughness() const { return m_roughness; }
    const float& GetMetallic() const { return m_metallic; }
    const Color& GetEmissiveColor() const { return m_emissiveColor; }

    const ShaderFlags& GetShaderFlags() const { return m_shaderFlags; }
    ShaderFlags& GetMutableShaderFlags() { return m_shaderFlags; }

    /// <summary>
    /// 指定されたスロットにテクスチャを設定し、アロケータからデスクリプタを確保します。
    /// </summary>
    /// <param name="slot">テクスチャを設定するスロット</param>
    /// <param name="texture">設定するテクスチャ</param>
    /// <param name="allocator">SRVを確保するためのディスクリプタアロケータ</param>
    void SetTexture(TextureSlot slot, Texture2D* texture);

    Texture2D* GetTexture(TextureSlot slot) const;
};