#pragma once

#include "Mesh.h"
#include "Material.h"

/// <summary>
/// 3D���f���A�Z�b�g���`���܂��B
/// �`��f�[�^�iMesh�j�ƕ\�ʃf�[�^�iMaterial�j�̃R���e�i�ł��B
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