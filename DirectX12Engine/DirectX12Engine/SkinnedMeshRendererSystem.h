#pragma once

struct SkinnedObjectConstantsLayout
{
	Matrix4x4 worldMatrix;
	Color diffuseColor;
	Color specularColor;
	float shininess;
	Vector3 padding;
	static constexpr int MAX_BONES = 128; // �V�F�[�_�[�ŃT�|�[�g����{�[���̍ő吔
	Matrix4x4 boneMatrices[MAX_BONES];
};

class SkinnedMeshRendererSystem : public System
{
private:
	friend class Application;

	// �����_�����O�p�C�v���C���p�̃��\�[�X
	static inline ComPtr<ID3D12PipelineState> m_graphicsPipelineState;
	static inline ComPtr<ID3D12RootSignature> m_rootSignature;

	// �����O�o�b�t�@�Ƃ��Ďg�p����I�u�W�F�N�g�萔�o�b�t�@
	static inline ComPtr<GraphicsBuffer> m_objectConstantBufferRing;
	static inline ComPtr<GraphicsBuffer> m_sceneConstantBuffer;
	static inline BYTE* m_mappedObjectConstants = nullptr;
	static inline UINT m_currentObjectBufferIndex = 0;

private:
	static void StaticConstructor();
	static void StaticDestructor();

public:
	void Start(ComponentManager& cm, World& world) override;

	void Draw(ComponentManager& cm, World& world) override;
};