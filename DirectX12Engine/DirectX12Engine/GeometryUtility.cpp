#include "GeometryUtility.h"
#include "SystemList.h"

void GeometryUtility::CalculateFrustumPlanes(const Matrix4x4& worldToProjectionMatrix, Plane planes[6])
{
    ExtractPlanes(planes, worldToProjectionMatrix);
}


void GeometryUtility::CalculateFrustumPlanes(World& world, Transform& transform, Camera& camera,Plane planes[6])
{
    const Matrix4x4& view = world.GetSystem<CameraSystem>()->GetWorldToCameraMatrix(transform, world);
    const Matrix4x4& proj = world.GetSystem<CameraSystem>()->GetProjectionMatrix(camera);
    CalculateFrustumPlanes(view * proj, planes);
}

bool GeometryUtility::RaycastTriangle(const Ray& ray, const Vector3& A, const Vector3& B, const Vector3& C, float maxDistance, float* enter)
{
    // まず平面と光線の交点を求める。
    Plane plane(A, B, C);

    float t;
    if (!plane.Raycast(ray, t))
    {
        // 平面と光線が平行の場合
        return false;
    }

    // 交点までの距離が遠すぎる場合は「交点なし」扱いにする
    if (t > maxDistance)
    {
        return false;
    }

    // 交点Pを求める
    const Vector3 P = ray.GetOrigin() + ray.GetDirection() * t;

    // 交点Pが三角形の内部に存在するかを調べる。(内外判定)

    // 交点Pから三角形の各頂点に向かうベクトルを求める。
    const Vector3 PA = A - P;
    const Vector3 PB = B - P;
    const Vector3 PC = C - P;

    // 外積計算
    const Vector3 PAxPB = Vector3::Cross(PA, PB);
    const Vector3 PBxPC = Vector3::Cross(PB, PC);
    const Vector3 PCxPA = Vector3::Cross(PC, PA);

    // どうやって2つのベクトルが同じ方向かを調べる？
    // 同じ方向  ⇒  成す角 0°  ⇒  (内積の値 > 0)
    if (Vector3::Dot(PAxPB, PBxPC) <= 0)
    {
        return false;
    }

    if (Vector3::Dot(PBxPC, PCxPA) <= 0)
    {
        return false;
    }

    // 全て同じ方向を向いているので交点Pは三角形の内部に存在する。
    if (enter != nullptr)
    {
        *enter = t;
    }

    return true;
}

bool GeometryUtility::SphereVSSphere(const Vector3& centerA, float radiusA, const Vector3& centerB, float radiusB)
{
    Vector3 delta = centerA - centerB;
    float distanceSquared = delta.SqrMagnitude();
    float radiusSum = radiusA + radiusB;
    return distanceSquared <= (radiusSum * radiusSum);
}

bool GeometryUtility::AABBVSAABB(const Vector3& minA, const Vector3& maxA, const Vector3& minB, const Vector3& maxB)
{
	// 各軸で分離しているかをチェック
	if (maxA.x < minB.x || minA.x > maxB.x) return false; // X軸で分離
	if (maxA.y < minB.y || minA.y > maxB.y) return false; // Y軸で分離
	if (maxA.z < minB.z || minA.z > maxB.z) return false; // Z軸で分離

	// どの軸でも分離していないので衝突している
	return true;
}


void GeometryUtility::ExtractPlanes(Plane planes[6], const Matrix4x4& worldToProjectionMatrix)
{
    // 参考: https://www.gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf

    // 引数名が長いので参照変数で短くしたい
    const Matrix4x4& m = worldToProjectionMatrix;

    struct UnnormalizedPlane
    {
        Vector3 normal;
        float distance;
    };

    // 正規化されていない平面 (Direct3D用)
    const UnnormalizedPlane unnormalizedPlanes[6] =
    {
        { Vector3(m._14 + m._11, m._24 + m._21, m._34 + m._31), m._44 + m._41 },  // Left
        { Vector3(m._14 - m._11, m._24 - m._21, m._34 - m._31), m._44 - m._41 },  // Right
        { Vector3(m._14 + m._12, m._24 + m._22, m._34 + m._32), m._44 + m._42 },  // Down
        { Vector3(m._14 - m._12, m._24 - m._22, m._34 - m._32), m._44 - m._42 },  // Up
        { Vector3(m._13,         m._23,         m._33), m._43         },  // Near
        { Vector3(m._14 - m._13, m._24 - m._23, m._34 - m._33), m._44 - m._43 },  // Far
    };

    // 平面の正規化
    for (int i = 0; i < 6; i++)
    {
        // ノルムの逆数
        const float reciprocal = 1.0f / unnormalizedPlanes[i].normal.Magnitude();
        planes[i] = Plane(unnormalizedPlanes[i].normal * reciprocal, unnormalizedPlanes[i].distance * reciprocal);
    }
}

