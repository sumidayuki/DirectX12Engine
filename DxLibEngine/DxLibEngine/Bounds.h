#pragma once

// min�}�N������`�ς݂������ꍇ�͖���������
#ifdef min
#undef min
#endif

// max�}�N������`�ς݂������ꍇ�͖���������
#ifdef max
#undef max
#endif

// XYZ���ɕ��s�ȋ��E�{�b�N�X (AABB = Axis Aligned Bounding Box)
class Bounds
{
private:
    // ���S���W
    Vector3 m_center;

    // �ӂ̔����̒���
    Vector3 m_extents;

public:
    // �f�t�H���g�R���X�g���N�^
    Bounds() = default;

    // �R���X�g���N�^
    Bounds(const Vector3& center, const Vector3& size);

    // ���S���W��ݒ肵�܂��B
    void SetCenter(const Vector3& center) { m_center = center; }

    // ���S���W�����擾���܂��B
    const Vector3& GetCenter() const { return m_center; }

    // �o�E���f�B���O�T�C�Y��ݒ肵�܂��B
    void SetSize(const Vector3& size) { m_extents = size * 0.5f; }

    // �o�E���f�B���O�T�C�Y���擾���܂��B
    Vector3 GetSize() const { return m_extents * 2.0f; }

    // �o�E���f�B���O�T�C�Y�̔�����ݒ肵�܂��B
    void SetExtents(const Vector3& extents) { m_extents = extents; }

    // �o�E���f�B���O�T�C�Y�̔������擾���܂��B
    const Vector3& GetExtents() const { return m_extents; }

    // �o�E���f�B���O�{�b�N�X�̍ŏ��l��ݒ肵�܂��B
    void SetMin(const Vector3& min) { SetMinMax(min, GetMax()); }

    // �o�E���f�B���O�{�b�N�X�̍ŏ��l���擾���܂��B
    // ���center�ϐ�����extents�ϐ����������l�ƂȂ�܂��B
    Vector3 GetMin() const { return m_center - m_extents; }

    // �o�E���f�B���O�{�b�N�X�̍ő�l��ݒ肵�܂��B
    void SetMax(const Vector3& max) { SetMinMax(GetMin(), max); }

    // �o�E���f�B���O�{�b�N�X�̍ő�l���擾���܂��B
    // ���center�ϐ���extents�ϐ��𑫂����l�ƂȂ�܂��B
    Vector3 GetMax() const { return m_center + m_extents; }

    // �o�E���f�B���O�{�b�N�X��̍ł��߂��_���擾���܂��B
    // ���� point ���o�E���f�B���O�{�b�N�X�����ɂ���ꍇ�͂��̂܂� point ��Ԃ��܂��B
    Vector3 ClosestPoint(const Vector3& point) const;

    // ���� point ���o�E���f�B���O�{�b�N�X�����Ɋ܂܂�Ă���ꍇ�� true ��Ԃ��܂��B
    bool Contains(const Vector3& point) const;

    // �w�肵�� amount �ɂ���� size ��傫�����܂��B
    void Expand(float amount);

    // ���̃o�E���f�B���O�{�b�N�X������ other �ƌ������Ă���ꍇ�� true ��Ԃ��܂��B
    bool Intersects(const Bounds& other) const;

    // �ŏ��l�ƍő�l���w�肵�ăo�E���f�B���O�{�b�N�X��ݒ肵�܂��B
    void SetMinMax(const Vector3& min, const Vector3& max);

    // ���� point �Ƃ̍ŏ�������2�悵���l��Ԃ��܂��B
    float SqrDistance(const Vector3& point) const;

    // ���̃o�E���f�B���O�{�b�N�X������ other �Ɠ������ꍇ�� true ��Ԃ��܂��B
    bool operator==(const Bounds& other);

    // ���̃o�E���f�B���O�{�b�N�X������ other �Ɠ������Ȃ��ꍇ�� true ��Ԃ��܂��B
    bool operator!=(const Bounds& other);
};

