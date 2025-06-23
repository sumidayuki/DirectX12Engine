#pragma once

/// <summary>
/// プロトタイプを管理します。
/// シングルトンクラスです。
/// </summary>
class ProtoTypeManager
{
private:
	static inline ProtoTypeManager* m_singleton = nullptr;

	std::unordered_map<std::string, ProtoType> m_ptototypes;

private:
	ProtoTypeManager();

	ProtoTypeManager(const ProtoTypeManager& source);

	ProtoTypeManager(ProtoTypeManager&& source);

	~ProtoTypeManager();

public:
	static void CreateSingleton();

	static ProtoTypeManager* GetInstance() { return m_singleton; }

	static void DestroySingleton();

public:
	void Register(const std::string& name, const ProtoType& proto);

	const ProtoType* Get(const std::string& name) const;
};