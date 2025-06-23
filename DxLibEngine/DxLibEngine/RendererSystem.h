#pragma once

/// <summary>
/// �����_�����O���s���V�X�e���ł��B
/// transform �� Z�l ���g���Đ[�x��\���܂��B
/// </summary>
class RendererSystem : public System
{
private:
    // �����o�ϐ��̐錾
    std::unique_ptr<View<RenderCommand, Transform>> m_view;

    std::vector<std::tuple<RenderCommand, Transform>> m_commands;

private:
    // �����o�֐��̐錾
    void RenderSprite(const Sprite& sprite, const Transform& transform);

    void RenderCircle(const Circle& circle, const Transform& transform);

    void RenderBox(const Box& box, const Transform& transform);

    void RenderTriangle(const Triangle& triangle, const Transform& transform);

private:
    // �I�[�o���C�h
    void Start(ComponentManager& cm, World& world) override;

    void Draw(ComponentManager& cm, World& world) override;
};