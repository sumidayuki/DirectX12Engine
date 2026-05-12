#include "ButtonFunctionRegistry.h"

ButtonFunctionRegistry::ButtonFunctionRegistry()
{
	// タイトルシーンに遷移
	RegisterFunction("ToTitleScene", []() {
		SceneManager::ChangeScene("TitleScene");
	});
}

void ButtonFunctionRegistry::RegisterFunction(const std::string& name, const std::function<void()>& function)
{
	m_functions[name] = function;
}

std::function<void()> ButtonFunctionRegistry::GetFunction(const std::string& name) const
{
	return std::function<void()>();
}
