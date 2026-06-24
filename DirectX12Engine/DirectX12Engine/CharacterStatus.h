#pragma once

/// <summary>
/// キャラクターのステータスを汎用的に保持するコンポーネントです。
/// </summary>
struct CharacterStatus : IComponentData
{
	std::unordered_map<std::string, float>       floats;
	std::unordered_map<std::string, int>         ints;
	std::unordered_map<std::string, bool>        bools;
	std::unordered_map<std::string, Vector3>     vectors;
	std::unordered_map<std::string, std::string> strings;
};