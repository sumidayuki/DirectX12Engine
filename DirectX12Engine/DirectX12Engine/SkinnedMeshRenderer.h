#pragma once

/// <summary>
/// スキニングされたメッシュのレンダリングに必要な情報を保持するコンポーネント。
/// 静的なMeshRendererとは区別されます。
/// </summary>
struct SkinnedMeshRenderer
{
	// 描画するメッシュ
	// このメッシュはボーン情報を含んでいる必要があります。
	ComPtr<Mesh> mesh = nullptr;

	// 描画に使用するマテリアルにリスト
	// Meshのサブメッシュインデックスと対応します。
	std::vector<ComPtr<Material>> materials;

	// メッシュに対応するスケルトン
	ComPtr<Skeleton> skeleton = nullptr;

	// このレンダラーのルートボーンとなるエンティティ
	// これにより、モデルの一部(剣など)を別のエンティティ階層に追従させることが可能になります。
	// 通常は、このコンポーネントを持つエンティティ自身です。
	Entity rootBoneEntity;
};