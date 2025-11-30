#pragma once

// ジオメトリ系の補助クラス
class GeometryUtility
{
public:
    // (view * projection)行列からフラスタムを構成する平面配列を作成します。
    static void CalculateFrustumPlanes(const Matrix4x4& worldToProjectionMatrix, Plane planes[6]);

    // カメラからフラスタムを構成する平面配列を作成します。
    static void CalculateFrustumPlanes(World& world, Transform& transform, Camera& camera, Plane planes[6]);

    static bool RaycastTriangle(const Ray& ray, const Vector3& A, const Vector3& B, const Vector3& C, float maxDistance = Mathf::Infinity, float* enter = nullptr);

	static bool SphereVSSphere(const Vector3& centerA, float radiusA, const Vector3& centerB, float radiusB);

    static bool AABBVSAABB(const Vector3& minA, const Vector3& maxA, const Vector3& minB, const Vector3& maxB);

private:
    // (view * projection)行列から平面配列を作成します。
    static void ExtractPlanes(Plane planes[6], const Matrix4x4& worldToProjectionMatrix);
};

