#pragma once

/// <summary>
/// メッシュのレンダリング方法を定義します。
/// </summary>
struct MeshRenderer
{
    // 描画に使用するマテリアルのリスト。
    // Meshのサブメッシュインデックスと対応します。
    std::vector<ComPtr<Material>> materials;
};