#pragma once
#include "TextureDimension.h"
#include "TextureWrapMode.h"
#include "FilterMode.h"

/// <summary>
/// テクスチャの基底クラスです。
/// </summary>
class Texture : public Reference
{
public:
    static inline const int GenerateAllMips = -1;

protected:
    std::string         m_name;             // テクスチャ名
    TextureDimension    m_dimension;        // テクスチャの次元
    int                 m_width;            // ピクセル単位の横幅
    int                 m_height;           // ピクセル単位の高さ
    int                 m_anisoLevel;       // 異方性フィルタリングレベル
    TextureWrapMode     m_wrapModeU;        // U軸方向のラップモード
    TextureWrapMode     m_wrapModeV;        // V軸方向のラップモード
    TextureWrapMode     m_wrapModeW;        // W軸方向のラップモード
    FilterMode          m_filterMode;       // フィルタモード
    float               m_mipMapBias;       // ミップマップバイアス
    bool                m_isReadable;       // ピクセルの読み取りが可能な場合は true

protected:
    // デフォルトコンストラクタ
    Texture();

    // 仮想デストラクタ
    virtual ~Texture() = default;

public:
    // ピクセル単位のテクスチャの横幅を取得します。
    int GetWidth() const { return m_width; }

    // ピクセル単位のテクスチャの高さを取得します。
    int GetHeight() const { return m_height; }

    // テクスチャの異方性フィルタリングのレベルを取得します。
    int GetAnisoLevel() const { return m_anisoLevel; }

    // テクスチャの次元を取得します。
    TextureDimension GetDimension() const { return m_dimension; }

    // テクスチャラップモードを設定します。 (この関数はU,V,W全てに同じラップモードを設定します)
    void SetWrapMode(TextureWrapMode wrapMode);

    // テクスチャU座標に対するラップモードを設定します。
    void SetWrapModeU(TextureWrapMode wrapMode) { m_wrapModeU = wrapMode; }

    // テクスチャV座標に対するラップモードを設定します。
    void SetWrapModeV(TextureWrapMode wrapMode) { m_wrapModeV = wrapMode; }

    // テクスチャW座標に対するラップモードを設定します。
    void SetWrapModeW(TextureWrapMode wrapMode) { m_wrapModeW = wrapMode; }

    // テクスチャU座標に対するラップモードを取得します。
    TextureWrapMode GetWrapModeU() const { return m_wrapModeU; };

    // テクスチャV座標に対するラップモードを取得します。
    TextureWrapMode GetWrapModeV() const { return m_wrapModeV; };

    // テクスチャW座標に対するラップモードを取得します。
    TextureWrapMode GetWrapModeW() const { return m_wrapModeW; };

    // テクスチャのフィルタリングモードを取得します。
    FilterMode GetFilterMode() const { return m_filterMode; }

    // ミップマップのバイアスを取得します。
    float GetMipMapBias() const { return m_mipMapBias; }

    // CPUによるピクセルデータ読み込みが可能な場合は true を返します。
    bool IsReadable() const { return m_isReadable; }

    // テクスチャリソースへのネイティブポインタを取得します。
    virtual void* GetNativeTexturePtr() const = 0;
};