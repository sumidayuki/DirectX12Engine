#pragma once

/// <summary>
/// �I�u�W�F�N�g�̕\�ʂ̌����ڂ��`����A�Z�b�g�B
/// �F�A�e�N�X�`���A�V�F�[�_�[�̃p�����[�^�Ȃǂ��Ǘ����܂��B
/// </summary>
class Material : public Reference
{
public:
    // �V�F�[�_�[���̃e�N�X�`���X���b�g�ɑΉ������邽�߂̃C���f�b�N�X
    enum class TextureSlot
    {
        Diffuse = 0,  // ��{�F�e�N�X�`�� (t0)
        Normal,       // �@���}�b�v (t1)
        Specular,     // ���ʔ��˃}�b�v (t2)
        // �K�v�ɉ����đ��̃}�b�v���ǉ�
        Max
    };

private:
    // --- Material Properties ---
    Color m_diffuseColor;   // �g�U���ːF
    Color m_specularColor;  // ���ʔ��ːF

    // --- Textures ---
    // �e�N�X�`���ւ̃|�C���^�i�Q�ƃJ�E���g�̂��߁j
    ComPtr<Texture2D> m_textures[(int)TextureSlot::Max];
    // �e�e�N�X�`����SRV�f�X�N���v�^��GPU�n���h��
    D3D12_GPU_DESCRIPTOR_HANDLE m_textureHandles[(int)TextureSlot::Max];

public:
    Material();
    ~Material() = default;

    // --- Colors ---
    void SetDiffuseColor(const Color& color) { m_diffuseColor = color; }
    const Color& GetDiffuseColor() const { return m_diffuseColor; }

    void SetSpecularColor(const Color& color) { m_specularColor = color; }
    const Color& GetSpecularColor() const { return m_specularColor; }

    // --- Textures ---
    /// <summary>
    /// �w�肳�ꂽ�X���b�g�Ƀe�N�X�`����ݒ肵�A�A���P�[�^����f�X�N���v�^���m�ۂ��܂��B
    /// </summary>
    /// <param name="slot">�e�N�X�`����ݒ肷��X���b�g</param>
    /// <param name="texture">�ݒ肷��e�N�X�`��</param>
    /// <param name="allocator">SRV���m�ۂ��邽�߂̃f�B�X�N���v�^�A���P�[�^</param>
    void SetTexture(TextureSlot slot, Texture2D* texture, DescriptorAllocator* allocator);

    Texture2D* GetTexture(TextureSlot slot) const { return m_textures[(int)slot].Get(); }

    // --- DirectX 12 Getters ---
    /// <summary>
    /// �w�肳�ꂽ�X���b�g�̃e�N�X�`���ɑΉ�����GPU�f�X�N���v�^�n���h�����擾���܂��B
    /// </summary>
    D3D12_GPU_DESCRIPTOR_HANDLE GetGpuDescriptorHandle(TextureSlot slot) const;
};