#pragma once

/// <summary>
/// 3Dメッシュをレンダリングするためのコンポーネントです。
/// 描画するメッシュや色などの情報を持ちます。
/// </summary>
struct MeshRenderer
{
    std::vector<ComPtr<Mesh>>     meshes;
    std::vector<ComPtr<Material>> materials;

    // オブジェクトごとの定数バッファ
    ComPtr<GraphicsBuffer> constantBuffer = nullptr;
};