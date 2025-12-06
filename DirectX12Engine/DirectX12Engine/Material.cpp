#include "Material.h"
#include "Graphics.h" // GetD3D12Device() のためにインクルード

Material::Material()
    : m_baseColor(Color::white)
    , m_roughness(1.0f)
    , m_metallic(0.0f)
    , m_emissiveColor(Color::black)
{
    // 全てのポインタとハンドルをゼロクリア
    for (int i = 0; i < (int)TextureSlot::Max; ++i)
    {
        m_textures[i] = nullptr;
    }
}

void Material::SetTexture(TextureSlot slot, Texture2D* texture)
{
    int slotIndex = (int)slot;
    if (slotIndex < 0 || slotIndex >= (int)TextureSlot::Max)
    {
        assert(false);
        return;
    }

    // テクスチャへのポインタを更新
    m_textures[slotIndex] = texture;
}

Texture2D* Material::GetTexture(TextureSlot slot) const
{
    if (m_textures[(int)slot])
    {
        return m_textures[(int)slot].Get();
    }

    return nullptr;
}
