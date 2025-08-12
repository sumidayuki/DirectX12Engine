#include "ResourceManager.h"

Model* ResourceManager::GetModel(std::string path)
{
	// ���ɓǂݍ��܂�Ă���`�F�b�N
	auto it = m_models.find(path);
	if (it != m_models.end())
	{
		// �L���b�V���ɂ���΂����Ԃ��B
		return it->second.Get();
	}

	// �L���b�V���ɂȂ���΁A�V�����ǂݍ���
	ModelImporter importer;
	ComPtr<Model> newModel = importer.Import(path);
	if (!newModel)
	{
		// �ǂݍ��ݎ��s
		return nullptr;
	}

	// �ǂݍ��񂾃��f�����L���b�V���ɕۑ����A���̃|�C���^��Ԃ��B
	m_models[path] = newModel;
	return newModel.Get();
}
