#pragma once

enum class TextureImporterMipFilter
{
    BoxFilter,
    KaiserFilter,
};


enum class TextureImporterType
{
    Default,        // �S�Ẵe�N�X�`���ɓK�p������ʓI�Ȑݒ肪�s���܂��B
    NormalMap,      // ���A���^�C���@���}�b�v�ɓK�����ݒ肪�s���܂��B
    GUI,            // HUD / GUI �ɓK�����ݒ���s���܂��B
    Sprite,         // Sprite �O���t�B�b�N�X�Ƃ��� Texture ���g�p����ꍇ�ɑI�����܂��B
    Cursor,         // �J�[�\���ɓK�����ݒ���s���܂��B
    Cookie,         // ���C�g�̃N�b�L�[�ɓK�����ݒ���s���܂��B
    Lightmap,       // ���C�g�}�b�v�ɓK�����ݒ���s���܂��B
    SingleChannel,  // �V���O���`�����l���e�N�X�`���ɓK�����ݒ���s���܂��B
};


enum class TextureImporterNPOTScale
{
    None,           // �X�P�[�����O�����ɂ��Ƃ̃e�N�X�`���T�C�Y���g�p���܂�
    ToNearest,      // 2�ׂ̂���ő傫�����ɃX�P�[�����O���܂�
    ToLarger,       // ��ԋ߂�2�ׂ̂���̃T�C�Y�ɃX�P�[�����O���܂�
    ToSmaller,      // 2 �ׂ̂���ŏ��������ɃX�P�[�����O���܂�
};


enum class SpriteImportMode
{
    None,           // �O���t�B�b�N�X���X�v���C�g�ł͂���܂���
    Single,         // �X�v���C�g���e�N�X�`�����玩���I�ɒ��o���ꂽ�P��̉摜�����Ƃ��Ďw�肵�܂�
    Multiple,       // �X�v���C�g�̓e�N�X�`�����璊�o���ꂽ�����̉摜�����ł��B
    Polygon,        // �X�v���C�g�͎��g�̗֊s������`���ꂽ���b�V���������Ă��܂��B
};


enum class SpriteMeshType
{
    FullRect,       // ��`�̓��b�V���Ɠ��������[�U�[���w�肵���X�v���C�g�T�C�Y
    Tight,          // �s�N�Z���̃A���t�@�l����ɂ����^�C�g�ȃ��b�V���B�����̗]���ȃs�N�Z���͉\�Ȍ���N���b�v����܂��B
};


// �e�N�X�`���C���|�[�^�[
class TextureImporter : public AssetImporter
{
private:
    // ��{�ݒ�
    TextureImporterType m_textureType;          // �e�N�X�`���̗��p���@
    TextureImporterNPOTScale m_npotScale;       // 2�̗ݏ�T�C�Y�ɃX�P�[�����O������@ (NPOT: Non Power Of Two)
    int m_maxTextureSize;                       // �ő�e�N�X�`���T�C�Y
    bool m_alphaIsTransparency;                 // ���߁A�񓧉߂̋��E�ɔ�������A�[�e�B�t�@�N�g���y������ꍇ�� true
    bool m_isReadable;                          // �ǂݍ��݉\�ɂ���ꍇ�� true

    // ���b�v���[�h�ݒ�
    TextureWrapMode m_wrapModeU;                // U�������̃e�N�X�`���A�h���b�V���O���[�h
    TextureWrapMode m_wrapModeV;                // V�������̃e�N�X�`���A�h���b�V���O���[�h
    TextureWrapMode m_wrapModeW;                // W�������̃e�N�X�`���A�h���b�V���O���[�h

    // �t�B���^�ݒ�
    FilterMode m_filterMode;                    // �C���|�[�g���̃t�B���^
    int m_anisoLevel;                           // �ٕ����t�B���^�̃��x�� (1�`9)

    // �~�b�v�}�b�v�ݒ�
    bool m_mipmapEnabled;                       // �~�b�v�}�b�v������L���ɂ���ꍇ�� true
    int m_mipmapFadeDistanceStart;              // �~�b�v�}�b�v�̃t�F�[�h�J�n����
    int m_mipmapFadeDistanceEnd;                // �~�b�v�}�b�v�̃t�F�[�h�I������
    float m_mipMapBias;                         // �قȂ�~�b�v���x������̃T���v�����u�����h����ۂ̃E�F�C�g�ɑ΂�����Z�l
    TextureImporterMipFilter m_mipmapFilter;    // �~�b�v�}�b�v�������̃t�B���^

    // �X�v���C�g�ݒ�
    SpriteImportMode m_spriteImportMode;        // �X�v���C�g�������̃C���|�[�g���@
    SpriteMeshType m_spriteMeshType;
    Vector4 m_spriteBorder;                     // �X�v���C�g�������̋��E��`
    Vector2 m_spritePivot;                      // �X�v���C�g�̃��[�J�����W�n�̊�_�ƂȂ�ʒu (SpriteMode::Single���̂ݗL��)
    float m_spritePixelsPerUnit;                // �X�v���C�g�������̃��[���h��Ԃ�1�P�ʂɊY������X�v���C�g�̉����ƍ���

public:
    // �f�t�H���g�R���X�g���N�^
    TextureImporter();

    // ���z�f�X�g���N�^
    virtual ~TextureImporter() override = default;

    // �e�N�X�`���̍ő�T�C�Y��ݒ肵�܂��B
    void SetMaxTextureSize(int maxSize) { m_maxTextureSize = maxSize; }

    // �e�N�X�`���̍ő�T�C�Y���擾���܂��B
    int GetMaxTextureSize() const { return m_maxTextureSize; }

    // �e�N�X�`���ٕ̈����t�B���^�����O�̃��x����ݒ肵�܂��B
    void SetAnisoLevel(int anisoLevel) { m_anisoLevel = anisoLevel; }

    // �e�N�X�`���ٕ̈����t�B���^�����O�̃��x�����擾���܂��B
    int GetAnisoLevel() const { return m_anisoLevel; }

    // �񋟂���Ă���A���t�@�`���l���������ł���ꍇ�́A
    // �����L���ɂ��ĐF�����O�Ƀt�B���^�����O�����邱�ƂŁA�t�B���^�����O�A�[�e�B�t�@�N�g��������܂��B
    void SetAlphaIsTransparency(bool enable) { m_alphaIsTransparency = enable; }

    // �񋟂���Ă���A���t�@�`���l���������ł���ꍇ�́A
    // �����L���ɂ��ĐF�����O�Ƀt�B���^�����O�����邱�ƂŁA�t�B���^�����O�A�[�e�B�t�@�N�g��������܂��B
    bool GetAlphaIsTransparency() const { return m_alphaIsTransparency; }

    // �e�N�X�`���f�[�^��ǂݎ��\�ɂ���ꍇ�� true ��ݒ肵�܂��B
    void SetReadable(bool isReadable) { m_isReadable = isReadable; }

    // �e�N�X�`���f�[�^���ǂݎ��\�ȏꍇ�� true ��Ԃ��܂��B
    bool IsReadable() const { return m_isReadable; }

    // �e�N�X�`���̃t�B���^�����O���[�h��ݒ肵�܂��B
    void SetFilterMode(FilterMode filterMode) { m_filterMode = filterMode; }

    // �e�N�X�`���̃t�B���^�����O���[�h���擾���܂��B
    FilterMode GetFilterMode() const { return m_filterMode; }

    // �~�b�v�}�b�v�̃t�B���^�����O���@��ݒ肵�܂��B
    void SetTextureImporterMipFilter(TextureImporterMipFilter mipmapFilter) { m_mipmapFilter = mipmapFilter; }

    // �~�b�v�}�b�v�̃t�B���^�����O���@���擾���܂��B
    TextureImporterMipFilter GetTextureImporterMipFilter() const { return m_mipmapFilter; }

    // �g�p�ړI�ɂ������e�N�X�`���̃^�C�v��ݒ肵�܂��B
    void SetTextureType(TextureImporterType textureType) { m_textureType = textureType; }

    // �g�p�ړI�ɂ������e�N�X�`���̃^�C�v���擾���܂��B
    TextureImporterType GetTextureType() const { return m_textureType; }

    // 2 �ׂ̂���T�C�Y�łȂ��e�N�X�`���̃X�P�[�����O���@��ݒ肵�܂��B
    void SetNPOTScale(TextureImporterNPOTScale npotScale) { m_npotScale = npotScale; }

    // 2 �ׂ̂���T�C�Y�łȂ��e�N�X�`���̃X�P�[�����O���@���擾���܂��B
    TextureImporterNPOTScale GetNPOTScale() const { return m_npotScale; }

    // �e�N�X�`�����b�v���[�h��ݒ肵�܂��B
    // U,V,W�S�Ăɓ������b�v���[�h��ݒ肵�܂��B
    void SetWrapMode(TextureWrapMode wrapMode);

    // �e�N�X�`��U���W�̃��b�v���[�h��ݒ肵�܂��B
    void SetWrapModeU(TextureWrapMode wrapMode) { m_wrapModeU = wrapMode; }

    // �e�N�X�`��V���W�̃��b�v���[�h��ݒ肵�܂��B
    void SetWrapModeV(TextureWrapMode wrapMode) { m_wrapModeV = wrapMode; }

    // �e�N�X�`��V���W�̃��b�v���[�h��ݒ肵�܂��B
    void SetWrapModeW(TextureWrapMode wrapMode) { m_wrapModeW = wrapMode; }

    // �e�N�X�`��U���W�̃��b�v���[�h���擾���܂��B
    TextureWrapMode GetWrapModeU() const { return m_wrapModeU; }

    // �e�N�X�`��V���W�̃��b�v���[�h���擾���܂��B
    TextureWrapMode GetWrapModeV() const { return m_wrapModeV; }

    // �e�N�X�`��W���W�̃��b�v���[�h���擾���܂��B
    TextureWrapMode GetWrapModeW() const { return m_wrapModeW; }

    // �X�v���C�g�̃C���|�[�g���̃��[�h��ݒ肵�܂��B
    void SetSpriteImportMode(SpriteImportMode spriteImportMode) { m_spriteImportMode = spriteImportMode; }

    // �X�v���C�g�̃C���|�[�g���̃��[�h���擾���܂��B
    SpriteImportMode GetSpriteImportMode() const { return m_spriteImportMode; }

    // �X�v���C�g���b�V���̎�ނ�ݒ肵�܂��B
    void SetSpriteMeshType(SpriteMeshType spriteMeshType) { m_spriteMeshType = spriteMeshType; }

    // �X�v���C�g���b�V���̎�ނ��擾���܂��B
    SpriteMeshType GetSpriteMehsType() const { return m_spriteMeshType; }

    // �������ꂽ�X�v���C�g�̃{�[�_�[��ݒ肵�܂��B
    void SetSpriteBorder(const Vector4& border) { m_spriteBorder = border; }

    // �������ꂽ�X�v���C�g�̃{�[�_�[���擾���܂��B
    const Vector4& GetSpriteBorder() const { return m_spriteBorder; }

    // Sprite�I�u�W�F�N�g�̍��W��Ԃ̒��S�_�̈ʒu��ݒ肵�܂��B
    void SetSpritePivot(const Vector2& spritePivot) { m_spritePivot = spritePivot; }

    // Sprite�I�u�W�F�N�g�̍��W��Ԃ̒��S�_�̈ʒu���擾���܂��B
    Vector2 GetSpritePivot() const { return m_spritePivot; }

    // ���[���h��ԍ��W��1�P�ʕ��ɑ�������X�v���C�g�̃s�N�Z������ݒ肵�܂��B
    void SetSpritePixelsPerUnit(float spritePixelsPerUnit) { m_spritePixelsPerUnit = spritePixelsPerUnit; }

    // ���[���h��ԍ��W��1�P�ʕ��ɑ�������X�v���C�g�̃s�N�Z�������擾���܂��B
    float GetSpritePixelsPerUnit() const { return m_spritePixelsPerUnit; }

    // �摜�t�@�C������͂��A�e�N�X�`�����\�[�X�Ƃ��ă��[�h���܂��B
    Texture2D* Import();

    // �摜�t�@�C������͂��A�e�N�X�`�����\�[�X�Ƃ��ă��[�h���܂��B
    Texture2D* Import(const wchar_t* path);

    // ��������̃f�[�^����C���|�[�g���邽�߂̐V�����֐���ǉ�
    Texture2D* Import(const void* data, size_t size);

    // ��������̔񈳏k�f�[�^����C���|�[�g����
    Texture2D* Import(
        int width,
        int height,
        DXGI_FORMAT format,
        const void* initialData,
        size_t rowPitch);
};

