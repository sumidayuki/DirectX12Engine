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
        m_textureHandles[i] = { 0 };
    }
}

void Material::SetTexture(TextureSlot slot, Texture2D* texture, DescriptorAllocator* allocator)
{
    int slotIndex = (int)slot;
    if (slotIndex < 0 || slotIndex >= (int)TextureSlot::Max)
    {
        assert(false);
        return;
    }

    // 1. テクスチャへのポインタを更新
    m_textures[slotIndex] = texture;

    // 2. テクスチャがnullptr、またはアロケータが無効なら、ハンドルをクリアして終了
    if (!texture || !allocator)
    {
        m_textureHandles[slotIndex] = { 0 };
        return;
    }

    // 3. SRV（シェーダーリソースビュー）のデスクリプタ（説明書）を作成
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = texture->GetNativeResource()->GetDesc().Format; // テクスチャのリソースからフォーマットを取得
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    // 4. アロケータにSRVの作成を依頼し、返されたGPUハンドルを保存
    m_textureHandles[slotIndex] = allocator->CreateSrv(texture->GetNativeResource(), srvDesc);
}

D3D12_GPU_DESCRIPTOR_HANDLE Material::GetGpuDescriptorHandle(TextureSlot slot) const
{
    int slotIndex = (int)slot;
    if (slotIndex < 0 || slotIndex >= (int)TextureSlot::Max)
    {
        assert(false);
        return { 0 };
    }
    return m_textureHandles[slotIndex];
}