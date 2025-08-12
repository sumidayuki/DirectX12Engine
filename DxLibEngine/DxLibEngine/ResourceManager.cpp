#include "ResourceManager.h"

Model* ResourceManager::GetModel(std::string path)
{
	// 既に読み込まれているチェック
	auto it = m_models.find(path);
	if (it != m_models.end())
	{
		// キャッシュにあればそれを返す。
		return it->second.Get();
	}

	// キャッシュになければ、新しく読み込む
	ModelImporter importer;
	ComPtr<Model> newModel = importer.Import(path);
	if (!newModel)
	{
		// 読み込み失敗
		return nullptr;
	}

	// 読み込んだモデルをキャッシュに保存し、そのポインタを返す。
	m_models[path] = newModel;
	return newModel.Get();
}
