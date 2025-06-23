#pragma once

enum class ProjectionType
{
	Perspective,
	Orthographic
};

/// <summary>
/// カメラを表します。
/// </summary>
struct Camera
{
    ProjectionType projectionType = ProjectionType::Perspective;

    // Perspective 用
    float fov = 60.0f;
    float aspectRatio = 16.0f / 9.0f;
    float nearClip = 0.1f;
    float farClip = 1000.0f;

    // Orthographic 用
    float orthoWidth = 1280.0f;
    float orthoHeight = 720.0f;

    Matrix4x4 viewMatrix = Matrix4x4::identity;
    Matrix4x4 projectionMatrix = Matrix4x4::identity;

    bool isMain = true;
};