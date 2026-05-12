#pragma once

/// <summary>
/// ボタンに使用する関数を管理するクラス
/// </summary>
class ButtonFunctionRegistry : public Singleton<ButtonFunctionRegistry>
{
	friend class Singleton<ButtonFunctionRegistry>;

private:
	std::unordered_map<std::string, std::function<void()>> m_functions;

private:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	ButtonFunctionRegistry();

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~ButtonFunctionRegistry() = default;

	void RegisterFunction(const std::string& name, const std::function<void()>& function);

public:
	std::function<void()> GetFunction(const std::string& name) const;
};