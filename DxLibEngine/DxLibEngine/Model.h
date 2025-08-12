#pragma once

class Model	: public Reference
{
	friend class World;
	friend class ModelImporter;

private:
	std::vector<ComPtr<Mesh>>		m_meshes;
	std::vector<ComPtr<Material>>	m_materials;
};