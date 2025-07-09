#pragma once

/// <summary>
/// 3D���b�V����`�悷��V�X�e���ł��B
/// </summary>
class MeshRendererSystem : public System
{
private:
    friend class Application;

    // ���L���\�[�X
    static inline ComPtr<ID3D12PipelineState> m_graphicsPipelineState;
    static inline ComPtr<ID3D12RootSignature> m_rootSignature;

    // ���L���\�[�X���������E�I�����܂�
    // �������̊֐��͍��� Application �N���X����Ăяo���K�v������܂��B
    static void StaticConstructor();
    static void StaticDestructor();

    // �萔�o�b�t�@�̃��C�A�E�g
    struct ConstantBufferLayout;

public:
    /// <summary>
    /// �����_���[�ɕ`�悷�郁�b�V����ݒ肵�܂��B
    /// </summary>
    void SetMesh(MeshRenderer* renderer, Mesh* mesh);

private:
    void Draw(ComponentManager& cm, World& world) override;
};