#include "EffectImporter.h"
#include "EffectManager.h"

Effect* EffectImporter::Import(const std::wstring& path)
{
	SetAssetPath(path.c_str());
	return Import();
}

Effect* EffectImporter::Import(const wchar_t* path)
{
	SetAssetPath(path);
	return Import();
}

Effect* EffectImporter::Import()
{
	// パスの取得
	const std::wstring& path = GetAssetPath();
	if (path.empty())
	{
		OutputDebugStringW(L"EffectImporter Error: Asset path is empty.\n");
		return nullptr;
	}

	// EffectManagerインスタンスの取得
	EffectManager* effectManager = EffectManager::GetInstance();
	if (!effectManager)
	{
		OutputDebugStringW(L"EffectImporter Error: EffectManager is null.\n");
		return nullptr;
	}

	// Effekseerのマネージャーを取得
	Effekseer::ManagerRef manager = effectManager->GetManager();
	if (!manager)
	{
		OutputDebugStringW(L"EffectImporter Error: Effekseer Manager is null.\n");
		return nullptr;
	}

	// エフェクトの読み込み
	Effekseer::EffectRef nativeEffect = Effekseer::Effect::Create(manager, reinterpret_cast<const EFK_CHAR*>(path.c_str()));
	if (!nativeEffect)
	{
		OutputDebugStringW((L"EffectImporter Error: Failed to load effect at" + path + L".\n").c_str());
		return nullptr;
	}

	// 拡張子を抜いたファイルネーム
	const std::string name = UTF16LEtoUTF8::Convert(GetFileNameWithoutExtension());

	// エフェクトの作成
	return Effect::Create(name, nativeEffect);
}
