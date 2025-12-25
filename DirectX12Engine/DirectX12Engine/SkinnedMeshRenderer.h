#pragma once

/// <summary>
/// スキニングされたメッシュのレンダリングに必要な情報を保持するコンポーネント。
/// 静的なMeshRendererとは区別されます。
/// </summary>
struct SkinnedMeshRenderer : IComponentData
{
	// 描画に使用するマテリアルのリスト
	// Meshのサブメッシュインデックスと対応します。
	std::vector<Material*> materials;

	Animator* animator = nullptr;

	// このレンダラーのルートボーンとなるエンティティ
	// これにより、モデルの一部(剣など)を別のエンティティ階層に追従させることが可能になります。
	// 通常は、このコンポーネントを持つエンティティ自身です。
	Entity rootBoneEntity;

	ComPtr<GraphicsBuffer> cbObject;             // オブジェクト用の定数バッファ（ワールド行列 + ボーン行列）
	ComPtr<GraphicsBuffer> sbMaterial;           // マテリアル用の構造化バッファ
};