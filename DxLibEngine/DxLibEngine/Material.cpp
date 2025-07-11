#include "Material.h"

Material::Material()
    : m_diffuseColor(Color::white),      // デフォルトの拡散反射色は白
    m_specularColor(Color(0.f, 0.f, 0.f, 1.f)), // デフォルトの鏡面反射色は黒
    m_descriptorHeap(nullptr)
{
    // 全てのテクスチャポインタをnullptrで初期化
    for (int i = 0; i < (int)TextureSlot::Max; ++i)
    {
        m_textures[i] = nullptr;
    }
}

/// <summary>
/// 指定されたスロットにテクスチャを設定します。
/// 内部でデスクリプタヒープを管理し、適切な場所にデスクリプタをコピーします。
/// </summary>
/// <param name="slot">テクスチャを設定するスロット</param>
/// <param name="texture">設定するテクスチャ</param>
void Material::SetTexture(TextureSlot slot, Texture2D* texture)
{
    int slotIndex = (int)slot;
    // スロットのインデックスが範囲内かチェック
    if (slotIndex < 0 || slotIndex >= (int)TextureSlot::Max)
    {
        // 範囲外の場合は何もしない（またはアサート）
        return;
    }

    // 1. テクスチャへのポインタを配列の適切な場所に保存
    m_textures[slotIndex] = texture;

    // 2. デスクリプタヒープがなければ、全スロット分を確保して作成する
    if (!m_descriptorHeap)
    {
        m_descriptorHeap.Attach(new DescriptorHeap(
            DescriptorHeapType::CBV_SRV_UAV,
            (int)TextureSlot::Max, // 全テクスチャスロット分のサイズを確保
            true)); // シェーダーから見えるようにする
    }

    // 3. テクスチャが有効な場合のみ、デスクリプタをコピーする
    if (m_textures[slotIndex])
    {
        // コピー先のハンドルを、スロットのインデックスを使って計算
        D3D12_CPU_DESCRIPTOR_HANDLE copyTo = m_descriptorHeap->GetCPUDescriptorHandle(slotIndex);

        // コピー元のハンドルを取得
        //（各テクスチャが自身のデスクリプタを保持していることを前提とする）
        D3D12_CPU_DESCRIPTOR_HANDLE copyFrom = m_textures[slotIndex]->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();

        // デバイスを取得してデスクリプタをコピー
        Graphics::GetD3D12Device()->CopyDescriptorsSimple(1, copyTo, copyFrom, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }
    // メモ: textureがnullptrの場合、このマテリアルのデスクリプタヒープ内の
    // 対応するデスクリプタは更新されませんが、シェーダー側で分岐すれば問題ありません。
}