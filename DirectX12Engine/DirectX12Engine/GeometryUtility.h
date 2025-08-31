#pragma once

// 授業で作りました。

// ジオメトリ系の補助クラス
class GeometryUtility
{
public:
    // (view * projection)行列からフラスタムを構成する平面配列を作成します。
    static void CalculateFrustumPlanes(const Matrix4x4& worldToProjectionMatrix, Plane planes[6]);

    // カメラからフラスタムを構成する平面配列を作成します。
    static void CalculateFrustumPlanes(World& world, Transform& transform, Camera& camera, Plane planes[6]);


private:
    // (view * projection)行列から平面配列を作成します。
    static void ExtractPlanes(Plane planes[6], const Matrix4x4& worldToProjectionMatrix);
};

