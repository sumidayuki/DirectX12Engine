#pragma once

class Sprite : public Reference
{
private:
    // �t�����h
    friend class SpriteRendererSystem;

    // �e���_�͈ʒu(x,y)�ƃe�N�X�`�����W(u,v)�����B
    struct Vertex
    {
        Vector2 position;
        Vector2 uv;
    };

private:
    std::string             m_name;                 // �e�N�X�`����
    ComPtr<Texture2D>       m_texture;              // �K�p����e�N�X�`��
    Rect                    m_rect;                 // �e�N�X�`����`
    Vector2                 m_pivot;                // �s�{�b�g
    float                   m_pixelsPerUnitX;       // 1�P�ʂ�����̃s�N�Z����
    float                   m_pixelsPerUnitY;       // 1�P�ʂ�����̃s�N�Z����
    Bounds                  m_bounds;               // ���f����ԓ��ł̋��E�{�b�N�X
    std::vector<Vector2>    m_vertices;             // ���_�ʒu(x,y)�̔z��
    std::vector<Vector2>    m_uv;                   // �e�N�X�`�����W(u,v)�̔z��
    std::vector<uint16_t>   m_triangles;            // �C���f�b�N�X�̔z��
    ComPtr<GraphicsBuffer>  m_vertexBuffer;         // ���_�o�b�t�@
    ComPtr<GraphicsBuffer>  m_indexBuffer;          // �C���f�b�N�X�o�b�t�@
    Rect                    m_textureRect;          // Tight�w�莞�ɕ␳���ꂽ�e�N�X�`����` (FillRect�̏ꍇ��rect�Ɠ����AUnity�͗�O�𔭐�������)
    Vector2                 m_textureRectOffset;    // 
    Vector2                 m_minUV;                // �ŏ�UV�l
    Vector2                 m_maxUV;                // �ő�UV�l

private:
    // �R���X�g���N�^
    Sprite(Texture2D* texture, const Rect& rect, const Vector2& pivot, float pixelsPerUnitX = 1.0f, float pixelsPerUnitY = 1.0f);

    // ���z�f�X�g���N�^
    ~Sprite() = default;

    /// <summary>
    /// �S�Ă̒��_���܂����`�̍ŏ��l�ƍő�l�����߂܂��B
    /// </summary>
    /// <param name="vertices"></param>
    /// <param name="vertexCount"></param>
    /// <param name="minPoint"></param>
    /// <param name="maxPoint"></param>
    static void CalcBoundingRect(const Vector2 vertices[], int vertexCount, Vector2& minPoint, Vector2& maxPoint);

    // ���_�o�b�t�@���擾���܂��B
    GraphicsBuffer* GetVertexBuffer() const { return m_vertexBuffer.Get(); }

    // �C���f�b�N�X�o�b�t�@���擾���܂��B
    GraphicsBuffer* GetIndexBuffer() const { return m_indexBuffer.Get(); }

public:
    /// <summary>
    /// �X�v���C�g��1�������܂��B
    /// </summary>
    /// <param name="texture"></param>
    /// <param name="rect"></param>
    /// <param name="pivot"></param>
    /// <param name="pixelsPerUnitX"></param>
    /// <param name="pixelsPerUnitY"></param>
    /// <returns></returns>
    static Sprite* Create(Texture2D* texture, const Rect& rect, const Vector2& pivot, float pixelsPerUnitX, float pixelsPerUnitY);

    // �֘A�t����ꂽ�e�N�X�`�����擾���܂��B
    Texture2D* GetTexture() const { return m_texture.Get(); }

    // �e�N�X�`����`���擾���܂��B
    const Rect& GetRect() const { return m_rect; }

    // �s�{�b�g���擾���܂��B
    const Vector2& GetPivot() const { return m_pivot; }

    // 1�P�ʂ�����̃s�N�Z�������擾���܂��B
    float GetPixelsPerUnitX() const { return m_pixelsPerUnitX; }

    // 1�P�ʂ�����̃s�N�Z�������擾���܂��B
    float GetPixelsPerUnitY() const { return m_pixelsPerUnitY; }

    // ���f����ԓ��ł̋��E�{�b�N�X���擾���܂��B
    const Bounds& GetBounds() const { return m_bounds; }

    // ���_�ʒu(x,y)�̔z����擾���܂��B
    const std::vector<Vector2>& GetVertices() const { return m_vertices; }

    // �e�N�X�`�����W(u,v)�̔z��
    const std::vector<Vector2>& GetUV() const { return m_uv; }

    // �C���f�b�N�X�̔z��
    const std::vector<uint16_t>& GetTriangles() const { return m_triangles; }

    /// <summary>
    /// �����f�[�^��j�����A�V�����W�I���g���f�[�^����ɂ��ăX�v���C�g���č쐬���܂��B
    /// </summary>
    /// <param name="vertices"></param>
    /// <param name="triangles"></param>
    void OverrideGeometry(const std::vector<Vector2>& vertices, const std::vector<uint16_t>& triangles);
};

