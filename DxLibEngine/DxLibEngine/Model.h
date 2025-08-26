#pragma once

#include "Mesh.h"
#include "Material.h"

/// <summary>
/// 3Dモデルアセットを定義します。
/// 形状データ（Mesh）と表面データ（Material）のコンテナです。
/// </summary>
class Model : public Reference
{
	friend class World;
	friend class ModelImporter;

private:
	ComPtr<Mesh>					m_mesh;
	std::vector<ComPtr<Material>>	m_materials;

public:
	Model() = default;
	virtual ~Model() override = default;
};