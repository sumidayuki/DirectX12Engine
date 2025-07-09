#pragma once

class CameraSystem : public System
{
	friend class World;

private:
	Camera* m_current = nullptr;

private:
	void SetCurrent(Camera* current) { m_current = current; }

public:
	/// <summary>
	/// 現在レンダリングに使用しているカメラを取得します。
	/// </summary>
	/// <returns></returns>
	Camera* GetCurrent() { return m_current; }

	const Matrix4x4& GetWorldToCameraMatrix(Transform& transform, World& world) const;

	const Matrix4x4& GetProjectionMatrix(Camera& camera) const;

	/// <summary>
	/// カメラ用定数バッファを取得します。
	/// </summary>
	/// <param name="camera"></param>
	/// <returns></returns>
	GraphicsBuffer* GetCameraBuffer(Camera& camera) const { return camera.cameraBuffer.Get(); }

private:
	void InternalRender(ComponentManager& cm, World& world) override;
};