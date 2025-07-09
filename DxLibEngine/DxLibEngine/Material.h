#pragma once

#include "Reference.h"
#include "Color.h"

/// <summary>
/// オブジェクトの表面の見た目を定義します。
/// 主にテクスチャや色などの情報を保持します。
/// </summary>
class Material : public Reference
{
public:
   // このマテリアルが使用するテクスチャ
   ComPtr<Texture2D>       m_mainTexture;

   // テクスチャがない場合などに使われる単色
   Color                   m_color;

   // シェーダーにテクスチャを渡すためのディスクリプタヒープ
   ComPtr<DescriptorHeap>  m_descriptorHeap;

public:
   Material();

   ~Material() = default;

   const Color GetColor() const { return m_color; }

   void SetColor(const Color color);

   Texture2D* GetMainTexture() const { return m_mainTexture.Get(); }

   DescriptorHeap* GetDescriptorHeap() const { return m_descriptorHeap.Get(); }

   /// <summary>
   /// このマテリアルにメインテクスチャを設定します。
   /// 同時に、シェーダーに渡すためのディスクリプタヒープも準備します。
   /// </summary>
   /// <param name="texture">設定する2Dテクスチャ</param>
   void SetMainTexture(Texture2D* texture);
};