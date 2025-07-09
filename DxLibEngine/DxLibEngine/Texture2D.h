#pragma once
#include "Texture.h"

// 2�����e�N�X�`��
class Texture2D : public Texture
{
    friend class TextureImporter;

private:
    ComPtr<ID3D12Resource>          m_nativeTexture;      // Direct3D12���\�[�X
    ComPtr<ID3D12Resource>          m_uploadBuffer;       // �A�b�v���[�h�o�b�t�@
    ComPtr<ID3D12DescriptorHeap>    m_descriptorHeap;     // �f�B�X�N���v�^�q�[�v

public:
    // �f�t�H���g�R���X�g���N�^
    Texture2D();

    // ���z�f�X�g���N�^
    virtual ~Texture2D();

    // �f�B�X�N���v�^�q�[�v���擾���܂��B
    ID3D12DescriptorHeap* GetDescriptorHeap() const { return m_descriptorHeap.Get(); }

    // Direct3D12���\�[�X���擾���܂��B
    void* GetNativeTexturePtr() const override { return m_nativeTexture.Get(); }
};
