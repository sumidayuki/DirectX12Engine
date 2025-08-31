#pragma once
#include "TextureDimension.h"
#include "TextureWrapMode.h"
#include "FilterMode.h"

/// <summary>
/// �e�N�X�`���̊��N���X�ł��B
/// </summary>
class Texture : public Reference
{
public:
    static inline const int GenerateAllMips = -1;

protected:
    std::string         m_name;             // �e�N�X�`����
    TextureDimension    m_dimension;        // �e�N�X�`���̎���
    int                 m_width;            // �s�N�Z���P�ʂ̉���
    int                 m_height;           // �s�N�Z���P�ʂ̍���
    int                 m_anisoLevel;       // �ٕ����t�B���^�����O���x��
    TextureWrapMode     m_wrapModeU;        // U�������̃��b�v���[�h
    TextureWrapMode     m_wrapModeV;        // V�������̃��b�v���[�h
    TextureWrapMode     m_wrapModeW;        // W�������̃��b�v���[�h
    FilterMode          m_filterMode;       // �t�B���^���[�h
    float               m_mipMapBias;       // �~�b�v�}�b�v�o�C�A�X
    bool                m_isReadable;       // �s�N�Z���̓ǂݎ�肪�\�ȏꍇ�� true

protected:
    // �f�t�H���g�R���X�g���N�^
    Texture();

    // ���z�f�X�g���N�^
    virtual ~Texture() = default;

public:
    // �s�N�Z���P�ʂ̃e�N�X�`���̉������擾���܂��B
    int GetWidth() const { return m_width; }

    // �s�N�Z���P�ʂ̃e�N�X�`���̍������擾���܂��B
    int GetHeight() const { return m_height; }

    // �e�N�X�`���ٕ̈����t�B���^�����O�̃��x�����擾���܂��B
    int GetAnisoLevel() const { return m_anisoLevel; }

    // �e�N�X�`���̎������擾���܂��B
    TextureDimension GetDimension() const { return m_dimension; }

    // �e�N�X�`�����b�v���[�h��ݒ肵�܂��B (���̊֐���U,V,W�S�Ăɓ������b�v���[�h��ݒ肵�܂�)
    void SetWrapMode(TextureWrapMode wrapMode);

    // �e�N�X�`��U���W�ɑ΂��郉�b�v���[�h��ݒ肵�܂��B
    void SetWrapModeU(TextureWrapMode wrapMode) { m_wrapModeU = wrapMode; }

    // �e�N�X�`��V���W�ɑ΂��郉�b�v���[�h��ݒ肵�܂��B
    void SetWrapModeV(TextureWrapMode wrapMode) { m_wrapModeV = wrapMode; }

    // �e�N�X�`��W���W�ɑ΂��郉�b�v���[�h��ݒ肵�܂��B
    void SetWrapModeW(TextureWrapMode wrapMode) { m_wrapModeW = wrapMode; }

    // �e�N�X�`��U���W�ɑ΂��郉�b�v���[�h���擾���܂��B
    TextureWrapMode GetWrapModeU() const { return m_wrapModeU; };

    // �e�N�X�`��V���W�ɑ΂��郉�b�v���[�h���擾���܂��B
    TextureWrapMode GetWrapModeV() const { return m_wrapModeV; };

    // �e�N�X�`��W���W�ɑ΂��郉�b�v���[�h���擾���܂��B
    TextureWrapMode GetWrapModeW() const { return m_wrapModeW; };

    // �e�N�X�`���̃t�B���^�����O���[�h���擾���܂��B
    FilterMode GetFilterMode() const { return m_filterMode; }

    // �~�b�v�}�b�v�̃o�C�A�X���擾���܂��B
    float GetMipMapBias() const { return m_mipMapBias; }

    // CPU�ɂ��s�N�Z���f�[�^�ǂݍ��݂��\�ȏꍇ�� true ��Ԃ��܂��B
    bool IsReadable() const { return m_isReadable; }

    // �e�N�X�`�����\�[�X�ւ̃l�C�e�B�u�|�C���^���擾���܂��B
    virtual void* GetNativeTexturePtr() const = 0;
};