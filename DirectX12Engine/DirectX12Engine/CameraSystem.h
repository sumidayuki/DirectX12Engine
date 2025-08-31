#pragma once

class CameraSystem : public System
{
	friend class World;

private:
	Camera* m_current = nullptr;
	Entity* m_currentEntity = nullptr;

private:
	void SetCurrent(Camera* current, Entity* entity) { m_current = current; m_currentEntity = entity; }

public:
	/// <summary>
	/// ���݃����_�����O�Ɏg�p���Ă���J�������擾���܂��B
	/// </summary>
	/// <returns></returns>
	Camera* GetCurrent() { return m_current; }

	Entity* GetCurrentEntity() { return m_currentEntity; }

	const Matrix4x4& GetWorldToCameraMatrix(Transform& transform, World& world) const;

	const Matrix4x4& GetProjectionMatrix(Camera& camera) const;

	/// <summary>
	/// �J�����p�萔�o�b�t�@���擾���܂��B
	/// </summary>
	/// <param name="camera"></param>
	/// <returns></returns>
	GraphicsBuffer* GetCameraBuffer(Camera& camera) const { return camera.cameraBuffer.Get(); }

private:
	void InternalRender(ComponentManager& cm, World& world) override;
};