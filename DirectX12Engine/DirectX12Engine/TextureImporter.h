#pragma once

enum class TextureImporterMipFilter
{
    BoxFilter,
    KaiserFilter,
};


enum class TextureImporterType
{
    Default,        // 全てのテクスチャに適用される一般的な設定が行えます。
    NormalMap,      // リアルタイム法線マップに適した設定が行えます。
    GUI,            // HUD / GUI に適した設定を行えます。
    Sprite,         // Sprite グラフィックスとして Texture を使用する場合に選択します。
    Cursor,         // カーソルに適した設定を行えます。
    Cookie,         // ライトのクッキーに適した設定を行えます。
    Lightmap,       // ライトマップに適した設定を行えます。
    SingleChannel,  // シングルチャンネルテクスチャに適した設定を行えます。
};


enum class TextureImporterNPOTScale
{
    None,           // スケーリングせずにもとのテクスチャサイズを使用します
    ToNearest,      // 2のべき乗で大きい方にスケーリングします
    ToLarger,       // 一番近い2のべき乗のサイズにスケーリングします
    ToSmaller,      // 2 のべき乗で小さい方にスケーリングします
};


enum class SpriteImportMode
{
    None,           // グラフィックスがスプライトではありません
    Single,         // スプライトがテクスチャから自動的に抽出された単一の画像部分として指定します
    Multiple,       // スプライトはテクスチャから抽出された複数の画像部分です。
    Polygon,        // スプライトは自身の輪郭線が定義されたメッシュを持っています。
};


enum class SpriteMeshType
{
    FullRect,       // 矩形はメッシュと等しいユーザーが指定したスプライトサイズ
    Tight,          // ピクセルのアルファ値を基にしたタイトなメッシュ。多くの余分なピクセルは可能な限りクロップされます。
};


// テクスチャインポーター
class TextureImporter : public AssetImporter
{
private:
    // 基本設定
    TextureImporterType m_textureType;          // テクスチャの利用方法
    TextureImporterNPOTScale m_npotScale;       // 2の累乗サイズにスケーリングする方法 (NPOT: Non Power Of Two)
    int m_maxTextureSize;                       // 最大テクスチャサイズ
    bool m_alphaIsTransparency;                 // 透過、非透過の境界に発生するアーティファクトを軽減する場合は true
    bool m_isReadable;                          // 読み込み可能にする場合は true

    // ラップモード設定
    TextureWrapMode m_wrapModeU;                // U軸方向のテクスチャアドレッシングモード
    TextureWrapMode m_wrapModeV;                // V軸方向のテクスチャアドレッシングモード
    TextureWrapMode m_wrapModeW;                // W軸方向のテクスチャアドレッシングモード

    // フィルタ設定
    FilterMode m_filterMode;                    // インポート時のフィルタ
    int m_anisoLevel;                           // 異方性フィルタのレベル (1～9)

    // ミップマップ設定
    bool m_mipmapEnabled;                       // ミップマップ生成を有効にする場合は true
    int m_mipmapFadeDistanceStart;              // ミップマップのフェード開始距離
    int m_mipmapFadeDistanceEnd;                // ミップマップのフェード終了距離
    float m_mipMapBias;                         // 異なるミップレベルからのサンプルをブレンドする際のウェイトに対する加算値
    TextureImporterMipFilter m_mipmapFilter;    // ミップマップ生成時のフィルタ

    // スプライト設定
    SpriteImportMode m_spriteImportMode;        // スプライト生成時のインポート方法
    SpriteMeshType m_spriteMeshType;
    Vector4 m_spriteBorder;                     // スプライト生成時の境界矩形
    Vector2 m_spritePivot;                      // スプライトのローカル座標系の基点となる位置 (SpriteMode::Single時のみ有効)
    float m_spritePixelsPerUnit;                // スプライト生成時のワールド空間の1単位に該当するスプライトの横幅と高さ

public:
    // デフォルトコンストラクタ
    TextureImporter();

    // 仮想デストラクタ
    virtual ~TextureImporter() override = default;

    // テクスチャの最大サイズを設定します。
    void SetMaxTextureSize(int maxSize) { m_maxTextureSize = maxSize; }

    // テクスチャの最大サイズを取得します。
    int GetMaxTextureSize() const { return m_maxTextureSize; }

    // テクスチャの異方性フィルタリングのレベルを設定します。
    void SetAnisoLevel(int anisoLevel) { m_anisoLevel = anisoLevel; }

    // テクスチャの異方性フィルタリングのレベルを取得します。
    int GetAnisoLevel() const { return m_anisoLevel; }

    // 提供されているアルファチャネルが透明である場合は、
    // これを有効にして色を事前にフィルタリングしすることで、フィルタリングアーティファクトを回避します。
    void SetAlphaIsTransparency(bool enable) { m_alphaIsTransparency = enable; }

    // 提供されているアルファチャネルが透明である場合は、
    // これを有効にして色を事前にフィルタリングしすることで、フィルタリングアーティファクトを回避します。
    bool GetAlphaIsTransparency() const { return m_alphaIsTransparency; }

    // テクスチャデータを読み取り可能にする場合は true を設定します。
    void SetReadable(bool isReadable) { m_isReadable = isReadable; }

    // テクスチャデータが読み取り可能な場合は true を返します。
    bool IsReadable() const { return m_isReadable; }

    // テクスチャのフィルタリングモードを設定します。
    void SetFilterMode(FilterMode filterMode) { m_filterMode = filterMode; }

    // テクスチャのフィルタリングモードを取得します。
    FilterMode GetFilterMode() const { return m_filterMode; }

    // ミップマップのフィルタリング方法を設定します。
    void SetTextureImporterMipFilter(TextureImporterMipFilter mipmapFilter) { m_mipmapFilter = mipmapFilter; }

    // ミップマップのフィルタリング方法を取得します。
    TextureImporterMipFilter GetTextureImporterMipFilter() const { return m_mipmapFilter; }

    // 使用目的にあったテクスチャのタイプを設定します。
    void SetTextureType(TextureImporterType textureType) { m_textureType = textureType; }

    // 使用目的にあったテクスチャのタイプを取得します。
    TextureImporterType GetTextureType() const { return m_textureType; }

    // 2 のべき乗サイズでないテクスチャのスケーリング方法を設定します。
    void SetNPOTScale(TextureImporterNPOTScale npotScale) { m_npotScale = npotScale; }

    // 2 のべき乗サイズでないテクスチャのスケーリング方法を取得します。
    TextureImporterNPOTScale GetNPOTScale() const { return m_npotScale; }

    // テクスチャラップモードを設定します。
    // U,V,W全てに同じラップモードを設定します。
    void SetWrapMode(TextureWrapMode wrapMode);

    // テクスチャU座標のラップモードを設定します。
    void SetWrapModeU(TextureWrapMode wrapMode) { m_wrapModeU = wrapMode; }

    // テクスチャV座標のラップモードを設定します。
    void SetWrapModeV(TextureWrapMode wrapMode) { m_wrapModeV = wrapMode; }

    // テクスチャV座標のラップモードを設定します。
    void SetWrapModeW(TextureWrapMode wrapMode) { m_wrapModeW = wrapMode; }

    // テクスチャU座標のラップモードを取得します。
    TextureWrapMode GetWrapModeU() const { return m_wrapModeU; }

    // テクスチャV座標のラップモードを取得します。
    TextureWrapMode GetWrapModeV() const { return m_wrapModeV; }

    // テクスチャW座標のラップモードを取得します。
    TextureWrapMode GetWrapModeW() const { return m_wrapModeW; }

    // スプライトのインポート時のモードを設定します。
    void SetSpriteImportMode(SpriteImportMode spriteImportMode) { m_spriteImportMode = spriteImportMode; }

    // スプライトのインポート時のモードを取得します。
    SpriteImportMode GetSpriteImportMode() const { return m_spriteImportMode; }

    // スプライトメッシュの種類を設定します。
    void SetSpriteMeshType(SpriteMeshType spriteMeshType) { m_spriteMeshType = spriteMeshType; }

    // スプライトメッシュの種類を取得します。
    SpriteMeshType GetSpriteMehsType() const { return m_spriteMeshType; }

    // 生成されたスプライトのボーダーを設定します。
    void SetSpriteBorder(const Vector4& border) { m_spriteBorder = border; }

    // 生成されたスプライトのボーダーを取得します。
    const Vector4& GetSpriteBorder() const { return m_spriteBorder; }

    // Spriteオブジェクトの座標空間の中心点の位置を設定します。
    void SetSpritePivot(const Vector2& spritePivot) { m_spritePivot = spritePivot; }

    // Spriteオブジェクトの座標空間の中心点の位置を取得します。
    Vector2 GetSpritePivot() const { return m_spritePivot; }

    // ワールド空間座標の1単位分に相当するスプライトのピクセル数を設定します。
    void SetSpritePixelsPerUnit(float spritePixelsPerUnit) { m_spritePixelsPerUnit = spritePixelsPerUnit; }

    // ワールド空間座標の1単位分に相当するスプライトのピクセル数を取得します。
    float GetSpritePixelsPerUnit() const { return m_spritePixelsPerUnit; }

    // 画像ファイルを解析し、テクスチャリソースとしてロードします。
    Texture2D* Import();

    // 画像ファイルを解析し、テクスチャリソースとしてロードします。
    Texture2D* Import(const wchar_t* path);

    // メモリ上のデータからインポートするための新しい関数を追加
    Texture2D* Import(const void* data, size_t size);

    // メモリ上の非圧縮データからインポートする
    Texture2D* Import(
        int width,
        int height,
        DXGI_FORMAT format,
        const void* initialData,
        size_t rowPitch);
};

