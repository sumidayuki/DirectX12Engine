#pragma once

/// <summary>
/// メッシュのレンダリング方法を定義します。
/// ワイヤーフレーム用レンダラーです。
/// </summary>
struct WFMeshRenderer : IComponentData
{
    // 描画に使用するマテリアルのリスト。
    // Meshのサブメッシュインデックスと対応します。
    std::vector<Material*> materials;
};