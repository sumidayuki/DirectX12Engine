#pragma once

// ���Ƃō��܂����B

// �W�I���g���n�̕⏕�N���X
class GeometryUtility
{
public:
    // (view * projection)�s�񂩂�t���X�^�����\�����镽�ʔz����쐬���܂��B
    static void CalculateFrustumPlanes(const Matrix4x4& worldToProjectionMatrix, Plane planes[6]);

    // �J��������t���X�^�����\�����镽�ʔz����쐬���܂��B
    static void CalculateFrustumPlanes(World& world, Transform& transform, Camera& camera, Plane planes[6]);


private:
    // (view * projection)�s�񂩂畽�ʔz����쐬���܂��B
    static void ExtractPlanes(Plane planes[6], const Matrix4x4& worldToProjectionMatrix);
};

