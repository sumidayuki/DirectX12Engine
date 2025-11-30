#include "Physics.h"
#include "DebugManager.h"

bool Physics::Raycast(World& world, const Ray& ray, RaycastHit& hit, float maxDistance, LayerMask targetMask)
{
    RaycastHit closestHit;
    closestHit.distance = maxDistance; // 最大距離で初期化
    
    View<Transform, MeshFilter, Layer> view(world);
    float minHitDistance = std::numeric_limits<float>::max();
    for (auto [entity, transform, meshFilter, layer] : view)
    {
        if ((layer.layer & targetMask) == 0)
        {
            // レイヤーマスクに含まれていない場合はスキップ
            continue;
        }
    
        // Meshオブジェクトを取得
        Mesh* mesh = meshFilter.mesh;
        if (!mesh) continue;
    
        // ワールド行列を取得
        const Matrix4x4& worldMatrix = TransformSystem::GetInstance()->GetLocalToWorldMatrix(transform);
    
        // 頂点配列とインデックス配列を直接取得
        const std::vector<Mesh::Vertex>& vertices = mesh->GetMutableVertices();
        const std::vector<uint32_t>& indices = mesh->GetIndices();
    
        // メッシュ内の全三角形を走査
        for (size_t i = 0; i < indices.size(); i += 3)
        {
            const Vector3& lv0 = vertices[indices[i]].position;
            const Vector3& lv1 = vertices[indices[i + 1]].position;
            const Vector3& lv2 = vertices[indices[i + 2]].position;
    
            Vector3 v0 = worldMatrix.MultiplyPoint(lv0);
            Vector3 v1 = worldMatrix.MultiplyPoint(lv1);
            Vector3 v2 = worldMatrix.MultiplyPoint(lv2);
    
            float t = 0.0f;
    
            if (GeometryUtility::RaycastTriangle(ray, v0, v1, v2, maxDistance, &t))
            {
                if (t < closestHit.distance)
                {
                    closestHit.distance = t;
                    closestHit.entity = entity;
    
                    // 衝突点を計算 P(t) = P0 + t * D
                    closestHit.point = ray.GetOrigin() + ray.GetDirection() * t;
    
                    // 衝突点の法線を計算（オプション：レイキャストでは三角形の法線を使用）
                    Vector3 edge1 = v1 - v0;
                    Vector3 edge2 = v2 - v0;
                    closestHit.normal = Vector3::Cross(edge1, edge2).Normalized();
                }
            }
        }
    }
        
    // 衝突を検出した場合、結果を hitInfo にコピーして true を返す
    if (closestHit.distance < maxDistance)
    {
        hit = closestHit;
        return true;
    }
    
    // 衝突しなかった
    return false;
}
