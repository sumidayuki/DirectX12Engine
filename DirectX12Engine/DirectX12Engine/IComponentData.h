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
/// - IComponentDataを継承していること
/// - memcpyで安全にコピーできること
/// - メモリレイアウトが標準であること
/// </summary>
template<typename T>
concept ComponentType =
std::is_base_of_v<IComponentData, T>&&
std::is_trivial_v<T>&&
std::is_standard_layout_v<T>;