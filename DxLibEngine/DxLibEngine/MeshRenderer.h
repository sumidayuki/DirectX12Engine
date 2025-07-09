#pragma once

/// <summary>
/// 3Dメッシュをレンダリングするためのコンポーネントです。
/// 描画するメッシュや色などの情報を持ちます。
/// </summary>
struct MeshRenderer
{
    ComPtr<Mesh> mesh = nullptr;
    ComPtr<Material> material = nullptr;

    // オブジェクトごとの定数バッファ
    ComPtr<GraphicsBuffer> constantBuffer = nullptr;

    bool isStarted = false;
};