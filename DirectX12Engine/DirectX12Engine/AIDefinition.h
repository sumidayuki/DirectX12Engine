#pragma once

using ConditionFunc = std::function<bool(Entity entity, World& world)>;

/// <summary>
/// AIステートマシンの遷移ルールを定義するデータ構造です。
/// </summary>
struct Transition
{
	uint32_t trigger;                       // いつ評価するか（"OnUpdate"_h 等）
	std::vector<ConditionFunc> conditions;  // 遷移条件のリスト（全て AND）
	uint32_t nextState;                     // 遷移先ステートのハッシュ値（高速比較用）
	std::string nextStateName;              // 遷移先ステートの名前（StatusAPI 書き込み用）
	InputKey input = InputKey::None;        // 遷移時に発行する入力
};