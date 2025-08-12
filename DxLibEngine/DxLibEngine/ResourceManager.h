#pragma once

class ResourceManager
{
private:
	static inline std::unordered_map<std::string, ComPtr<Model>> m_models;

public:
	static Model* GetModel(std::string path);

private:
	static void StaticConstructor();

	static void StaticDestructor();
};