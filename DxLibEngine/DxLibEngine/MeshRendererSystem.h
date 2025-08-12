#pragma once

struct SceneConstants
{
    int activeLightCount;
    float padding[3];
    Vector4 cameraWorldPosition;
};

// �I�u�W�F�N�g���Ɓi�}�e���A�����Ɓj�̒萔
struct ObjectConstantsLayout
{
    Matrix4x4 worldMatrix;
    Color     diffuseColor;
    Color     specularColor; // ���ʔ��˂̐F
    float     shininess;     // ����x
    float     obj_padding[3]; // �p�f�B���O
};

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

    static inline ComPtr<Texture2D> m_defaultWhiteTexture;

    // �萔�o�b�t�@�̃��C�A�E�g
    struct ConstantBufferLayout;

    ComPtr<GraphicsBuffer> m_sceneConstantBuffer;

public:
    static Texture2D* GetDefaultWhiteTexture() { return m_defaultWhiteTexture.Get(); }

public:
    /// <summary>
    /// �����_���[�ɕ`�悷�郁�b�V����ݒ肵�܂��B
    /// </summary>
    void SetMesh(MeshRenderer* renderer, Mesh* mesh);

private:
    void Start(ComponentManager& cm, World& world) override;

    void Draw(ComponentManager& cm, World& world) override;
};