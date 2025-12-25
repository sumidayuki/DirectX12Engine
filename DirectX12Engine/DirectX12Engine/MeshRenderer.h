#pragma once

/// <summary>
/// メッシュのレンダリング方法を定義します。
/// </summary>
struct MeshRenderer : IComponentData
{
    // 描画に使用するマテリアルのリスト。
    // Meshのサブメッシュインデックスと対応します。
    std::vector<Material*> materials;

    ComPtr<GraphicsBuffer> cbObject;             // オブジェクト用の定数バッファ
    ComPtr<GraphicsBuffer> sbMaterial;           // マテリアル用の構造化バッファ
};