#pragma once

struct SceneConstants
{
    int activeLightCount;
    float padding[3];
    Vector4 cameraWorldPosition;
};

// �I�u�W�F�N�g���Ɓi�}�e���A�����Ɓj�̒萔
// HLSL����cbuffer�ƈ�v�����A256�o�C�g�A���C�����g���l�������T�C�Y�ɂ��܂�
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

    static inline ComPtr<Texture2D> m_defaultWhiteTexture;

    // �����O�o�b�t�@�Ƃ��Ďg�p����I�u�W�F�N�g�萔�o�b�t�@
    static inline ComPtr<GraphicsBuffer> m_objectConstantBufferRing;
    // CPU���珑�����ނ��߂̃}�b�v�ς݃|�C���^
    static inline BYTE* m_mappedObjectConstants = nullptr;
    // ���݂̃t���[���ŕ`�悵���I�u�W�F�N�g�̐��i�����O�o�b�t�@�̃C���f�b�N�X�j
    static inline UINT m_currentObjectBufferIndex = 0;

    // �V�[�����Ƃ̒萔�o�b�t�@
    ComPtr<GraphicsBuffer> m_sceneConstantBuffer;

public:
    static Texture2D* GetDefaultWhiteTexture() { return m_defaultWhiteTexture.Get(); }

public:
    /// <summary>
    /// �����_���[�ɕ`�悷�郁�b�V����ݒ肵�܂��B
    /// </summary>
    void SetMesh(MeshRenderer* renderer, Mesh* mesh);

private:
    // ���L���\�[�X���������E�I�����܂�
    // �������̊֐��͍��� Application �N���X����Ăяo���K�v������܂��B
    static void StaticConstructor();
    static void StaticDestructor();

    void Start(ComponentManager& cm, World& world) override;

    void Draw(ComponentManager& cm, World& world) override;
};