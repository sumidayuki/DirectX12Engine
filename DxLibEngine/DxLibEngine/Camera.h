#pragma once

/// <summary>
/// カメラのレンダリング先のクリア方法を表す列挙型です。
/// </summary>
enum class CameraClearFlags
{
    Skybox,
    SolidColor,
    Depth,
    Nothing,
};

/// <summary>
/// カメラを表します。
/// </summary>
struct Camera
{
    CameraClearFlags        clearFlags = CameraClearFlags::SolidColor;
    Color                   backgroundColor = Color::cornflowerBlue;
    bool                    orthographic = false;
    float                   orthographicSize = 5.0f;
    float                   fieldOfView = 60.0f;
    float                   aspect = 1.777777;
    float                   nearClipPlane = 0.3f;
    float                   farClipPlane = 1000.0f;
    float                   depth = 0.0f;
    Rect                    viewportRect = Rect(0.0f, 0.0f, 1.0f, 1.0f);
    ComPtr<GraphicsBuffer>  cameraBuffer = nullptr;
    mutable Matrix4x4       projectionMatrix = Matrix4x4::identity;
    mutable bool            projectionMatrixIsDirty = true;

    bool                    isStarted = false;
};