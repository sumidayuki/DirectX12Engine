#pragma once

/// <summary>
/// �摜�t�@�C����`�悷��V�X�e���ł��B
/// </summary>
class SpriteRendererSystem : public System
{
private:
    friend class Application;

private:
    // �S�ẴX�v���C�g�����_���[�ŋ��L����I�u�W�F�N�g
    static inline ComPtr<ID3D12PipelineState> m_graphicsPipelineState;
    static inline ComPtr<ID3D12RootSignature> m_rootSignature;

    // ���L���\�[�X�����������܂��B
    // (���̊֐���Application�N���X����̂݌Ăяo����܂�)
    static void StaticConstructor();

    // ���L���\�[�X�̏I���������s���܂��B
    // (���̊֐���Application�N���X����̂݌Ăяo����܂�)
    static void StaticDestructor();

	// �萔�o�b�t�@�̃��������C�A�E�g��\���\����
	struct ConstantBufferLayout;

public:
    void SetSprite(SpriteRenderer* spriteRenderer, Sprite* sprite);

private:
	void Start(ComponentManager& cm, World& world) override;

	void Draw(ComponentManager& cm, World& world) override;
};