#include "CameraSystem.h"
#include "TransformSystem.h"

// �萔�o�b�t�@�̃��������C�A�E�g��\���\����
struct CameraLayout
{
	Matrix4x4 viewMatrix;
	Matrix4x4 projMatrix;
};

const Matrix4x4& CameraSystem::GetWorldToCameraMatrix(Transform& transform, World& world) const
{
	return world.GetSystem<TransformSystem>()->GetWorldToLocalMatrix(transform);
}

const Matrix4x4& CameraSystem::GetProjectionMatrix(Camera& camera) const
{
	if (camera.projectionMatrixIsDirty)
	{
		if (camera.orthographic)
		{
			// ���s���e
			const float halfWidth = camera.orthographicSize * camera.aspect;
			const float left = -halfWidth;
			const float right = halfWidth;
			const float bottom = -camera.orthographicSize;
			const float top = camera.orthographicSize;
			camera.projectionMatrix = Matrix4x4::Ortho(left, right, bottom, top, camera.nearClipPlane, camera.farClipPlane);
		}
		else
		{
			// �������e
			camera.projectionMatrix = Matrix4x4::Perspective(camera.fieldOfView, camera.aspect, camera.nearClipPlane, camera.farClipPlane);
		}
		camera.projectionMatrixIsDirty = false;
	}

	return camera.projectionMatrix;
}

void CameraSystem::InternalRender(ComponentManager& cm, World& world)
{
	View<Transform, Camera> view(cm);

	for (auto [entity, transform, camera] : view)
	{
		if (!camera.isStarted)
		{
			camera.cameraBuffer.Attach(new GraphicsBuffer(GraphicsBuffer::Target::Constant, GraphicsBuffer::UsageFlags::LockBufferForWrite, 1, sizeof(CameraLayout)));

			camera.isStarted = true;
		}

		// ���݂̃t���[�����\�[�X���擾
		FrameResource* frameResource = Graphics::GetCurrentFrameResource();

		// �R�}���h���X�g���擾
		ID3D12GraphicsCommandList* commandList = frameResource->GetCommandList();

		// �萔�o�b�t�@�ɏ�������
		CameraLayout* lockedPointer = (CameraLayout*)camera.cameraBuffer->LockBufferForWrite();
		lockedPointer->viewMatrix = GetWorldToCameraMatrix(transform, world).Transpose();
		lockedPointer->projMatrix = GetProjectionMatrix(camera).Transpose();
		camera.cameraBuffer->UnlockBufferAfterWrite();

		// �R�}���h�u�v���~�e�B�u�g�|���W�[��ύX����v���R�}���h���X�g�ɒǉ�
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// �R�}���h�u�V�U�[��`�z���ύX����v���R�}���h���X�g�ɒǉ�
		D3D12_RECT scissorRects[1];
		memset(scissorRects, 0, sizeof(scissorRects));
		scissorRects[0].left = 0;
		scissorRects[0].top = 0;
		scissorRects[0].right = Screen::GetWidth();
		scissorRects[0].bottom = Screen::GetHeight();
		commandList->RSSetScissorRects(_countof(scissorRects), scissorRects);

		// �R�}���h�u�r���[�|�[�g�z���ύX����v���R�}���h���X���ɒǉ�
		D3D12_VIEWPORT viewports[1];
		memset(viewports, 0, sizeof(viewports));
		viewports[0].MinDepth = D3D12_MIN_DEPTH;
		viewports[0].MaxDepth = D3D12_MAX_DEPTH;
		viewports[0].TopLeftX = Screen::GetWidth() * camera.viewportRect.x;
		viewports[0].TopLeftY = Screen::GetHeight() * (1.0f - camera.viewportRect.y - camera.viewportRect.height);
		viewports[0].Width   = Screen::GetWidth() * camera.viewportRect.width;
		viewports[0].Height  = Screen::GetHeight() * camera.viewportRect.height;
		commandList->RSSetViewports(_countof(viewports), viewports);

		// �J���[�o�b�t�@�Ɛ[�x�o�b�t�@
		D3D12_CPU_DESCRIPTOR_HANDLE handleRTV = frameResource->GetHandleRTV();
		D3D12_CPU_DESCRIPTOR_HANDLE handleDSV = frameResource->GetHandleDSV();

		// �����_�[�^�[�Q�b�g���N���A����ۂ̋�`�̈�
		D3D12_RECT clearRect;
		memset(&clearRect, 0, sizeof(clearRect));
		clearRect.left = (LONG)viewports[0].TopLeftX;
		clearRect.top = (LONG)viewports[0].TopLeftY;
		clearRect.right = (LONG)(viewports[0].TopLeftX + viewports[0].Width);
		clearRect.bottom = (LONG)(viewports[0].TopLeftY + viewports[0].Height);

		switch (camera.clearFlags)
		{
			case CameraClearFlags::Skybox:
			case CameraClearFlags::SolidColor:
			{
				// �R�}���h�u�����_�[�^�[�Q�b�g��P�F�œh��Ԃ��v���R�}���h���X�g�ɒǉ�
				commandList->ClearRenderTargetView(handleRTV, camera.backgroundColor, 1, &clearRect);
				commandList->ClearDepthStencilView(handleDSV, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 1, &clearRect);
			}
			break;

			case CameraClearFlags::Depth:
			{
				// �R�}���h�u�[�x�X�e���V���o�b�t�@���w�肵���l�œh��Ԃ��v���R�}���h���X�g�ɒǉ�
				commandList->ClearDepthStencilView(handleDSV, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 1, &clearRect);
			}
			break;
		}

		// �R�}���h�u�����_�[�^�[�Q�b�g�z��Ɛ[�x�X�e���V���o�b�t�@��ύX����v���R�}���h���X�g�ɒǉ�
		const D3D12_CPU_DESCRIPTOR_HANDLE renderTargets[] = { handleRTV };
		commandList->OMSetRenderTargets(_countof(renderTargets), renderTargets, TRUE, &handleDSV);

		// �r���[�t���X�^���p�̕��ʔz����쐬
		Plane planes[6];
		GeometryUtility::CalculateFrustumPlanes(world, transform, camera, planes);
	}
}