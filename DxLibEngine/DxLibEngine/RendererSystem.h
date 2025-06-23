#pragma once

/// <summary>
/// レンダリングを行うシステムです。
/// transform の Z値 を使って深度を表します。
/// </summary>
class RendererSystem : public System
{
private:
    // メンバ変数の宣言
    std::unique_ptr<View<RenderCommand, Transform>> m_view;

    std::vector<std::tuple<RenderCommand, Transform>> m_commands;

private:
    // メンバ関数の宣言
    void RenderSprite(const Sprite& sprite, const Transform& transform);

    void RenderCircle(const Circle& circle, const Transform& transform);

    void RenderBox(const Box& box, const Transform& transform);

    void RenderTriangle(const Triangle& triangle, const Transform& transform);

private:
    // オーバライド
    void Start(ComponentManager& cm, World& world) override;

    void Draw(ComponentManager& cm, World& world) override;
};