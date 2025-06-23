#include "CameraSystem.h"

/*
void CameraSystem::Update(ComponentManager& cm, World& world)
{
    View<Camera, Transform> view(cm);

    for (auto [entity, camera, transform] : view)
    {
        // View行列：Transformの逆行列
        camera.viewMatrix = transform.worldMatrix.Inverse();

        // Projection行列
        if (camera.projectionType == ProjectionType::Perspective)
        {
            camera.projectionMatrix = Matrix4x4::Perspective(
                camera.fov,
                camera.aspectRatio,
                camera.nearClip,
                camera.farClip
            );
        }
        else if (camera.projectionType == ProjectionType::Orthographic)
        {
            float halfW = camera.orthoWidth / 2.0f;
            float halfH = camera.orthoHeight / 2.0f;

            camera.projectionMatrix = Matrix4x4::Ortho(
                -halfW, halfW,
                -halfH, halfH,
                camera.nearClip,
                camera.farClip
            );
        }

        if (camera.isMain)
        {
            Vector3 pos = transform.position;
            Vector3 dir = Vector3::Scale(Vector3(transform.rotation.x, transform.rotation.y, transform.rotation.z), Vector3::forward).Normalized();
            Vector3 target = pos + dir;

            SetCameraPositionAndTarget_UpVecY
            (
                VGet(pos.x, pos.y, pos.z),
                VGet(target.x, target.y, target.z)
            );

            SetCameraNearFar(camera.nearClip, camera.farClip);
        }
    }
}
*/