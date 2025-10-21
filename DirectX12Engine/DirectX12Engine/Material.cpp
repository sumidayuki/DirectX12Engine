#include "Material.h"
#include "Graphics.h" // GetD3D12Device() のためにインクルード

Material::Material()
    : m_diffuseColor(Color::white),
    m_specularColor(Color(0.2f, 0.2f, 0.2f, 1.f)) // デフォルトの鏡面反射色は控えめな灰色に
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