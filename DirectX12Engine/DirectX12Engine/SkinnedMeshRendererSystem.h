#pragma once

struct SkinnedObjectConstantsLayout
{
	Matrix4x4 worldMatrix;
	Color diffuseColor;
	Color specularColor;
	float shininess;
	Vector3 padding;
	static constexpr int MAX_BONES = 256; // シェーダーでサポートするボーンの最大数
	Matrix4x4 boneMatrices[MAX_BONES];
};

class SkinnedMeshRendererSystem : public System
{
private:
	friend class Application;

	// レンダリングパイプライン用のリソース
	static inline ComPtr<ID3D12PipelineState> m_graphicsPipelineState;
	static inline ComPtr<ID3D12RootSignature> m_rootSignature;

	// リングバッファとして使用するオブジェクト定数バッファ
	static inline ComPtr<GraphicsBuffer> m_objectConstantBufferRing;
	static inline ComPtr<GraphicsBuffer> m_sceneConstantBuffer;
	static inline BYTE* m_mappedObjectConstants = nullptr;
	static inline UINT m_currentObjectBufferIndex = 0;

private:
	/// <summary>
	/// ボーンの階層を再帰的に更新します。
	/// </summary>
	/// <param name="bone"></param>
	/// <param name="parentTransform"></param>
	/// <param name="animation"></param>
	/// <param name="currentTime"></param>
	/// <param name="boneTransforms"></param>
	void CalculateBoneTransform(const Bone* bone, const Matrix4x4& parentTransform, const Animation* animation, float currentTime, std::unordered_map<std::string, Matrix4x4>& boneTransforms);

private:
	static void StaticConstructor();
	static void StaticDestructor();

public:
	void Start(ComponentManager& cm, World& world) override;

	void Draw(ComponentManager& cm, World& world) override;
};