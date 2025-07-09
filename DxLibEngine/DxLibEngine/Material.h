#pragma once

#include "Reference.h"
#include "Color.h"

/// <summary>
/// �I�u�W�F�N�g�̕\�ʂ̌����ڂ��`���܂��B
/// ��Ƀe�N�X�`����F�Ȃǂ̏���ێ����܂��B
/// </summary>
class Material : public Reference
{
public:
   // ���̃}�e���A�����g�p����e�N�X�`��
   ComPtr<Texture2D>       m_mainTexture;

   // �e�N�X�`�����Ȃ��ꍇ�ȂǂɎg����P�F
   Color                   m_color;

   // �V�F�[�_�[�Ƀe�N�X�`����n�����߂̃f�B�X�N���v�^�q�[�v
   ComPtr<DescriptorHeap>  m_descriptorHeap;

public:
   Material();

   ~Material() = default;

   const Color GetColor() const { return m_color; }

   void SetColor(const Color color);

   Texture2D* GetMainTexture() const { return m_mainTexture.Get(); }

   DescriptorHeap* GetDescriptorHeap() const { return m_descriptorHeap.Get(); }

   /// <summary>
   /// ���̃}�e���A���Ƀ��C���e�N�X�`����ݒ肵�܂��B
   /// �����ɁA�V�F�[�_�[�ɓn�����߂̃f�B�X�N���v�^�q�[�v���������܂��B
   /// </summary>
   /// <param name="texture">�ݒ肷��2D�e�N�X�`��</param>
   void SetMainTexture(Texture2D* texture);
};