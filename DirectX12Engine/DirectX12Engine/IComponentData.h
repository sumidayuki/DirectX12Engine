#pragma once

/// <summary>
/// Componentの共通基底です。
/// Componentという事を保証するためのマーカーです。
/// </summary>
struct IComponentData
{
};

/// <summary>
/// Componentとして登録可能な型を制約するconcept。
/// IComponentDataを継承していることを保証します。
/// </summary>
template<typename T>
concept ComponentType =
std::is_base_of_v<IComponentData, T>;