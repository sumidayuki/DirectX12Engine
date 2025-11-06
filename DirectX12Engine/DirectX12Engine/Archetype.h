#pragma once
#include "TypeInfo.h"
#include "IComponentData.h"

/// <summary>
/// Archetypeは「同じコンポーネント集合を持つEntity1体分のレイアウト」を表す構造体です。
/// 含まれるTypeInfoのリストを持ち、Chunkのレイアウトを決定します。
/// このArchetypeは型リストとオフセット情報を保持するだけのデータ構造であって、
/// 振る舞いを持たないためstructで定義しています。
/// </summary>
struct Archetype
{
    std::vector<TypeInfo> types;             // このArchetypeに含まれる型情報
    std::vector<size_t>   offsets;           // 各コンポーネントのメモリオフセット
    size_t                totalSize = 0;     // 1エンティティ分の合計サイズ

    /// <summary>
    /// Archetypeを構築します。TypeInfoをソートして一意化しています。
    /// </summary>
    Archetype(std::vector<TypeInfo> typeList)
    {
        // ソートして順序を一定に
        std::sort(
            typeList.begin(), 
            typeList.end(), 
            [](auto& a, auto& b) 
            {
                return a.hash < b.hash;
            }
        );

        // 重複を削除
        typeList.erase(std::unique(typeList.begin(), typeList.end(),
            [](auto& a, auto& b) { return a.hash == b.hash; }),
            typeList.end());

        types = std::move(typeList);

        // メモリオフセットを計算
        size_t offset = 0;
        for (const auto& t : types)
        {
            offset = (offset + t.alignment - 1) / t.alignment * t.alignment; // アライメント調整
            offsets.push_back(offset);
            offset += t.size;
        }
        totalSize = offset;
    }

    /// <summary>
    /// Archetype同士の比較（構成が同じか）をします。
    /// </summary>
    bool operator==(const Archetype& other) const noexcept
    {
        if (types.size() != other.types.size())
            return false;
        for (size_t i = 0; i < types.size(); ++i)
        {
            if (types[i].hash != other.types[i].hash)
                return false;
        }
        return true;
    }

    /// <summary>
    /// 特定のコンポーネント型を含むかどうかを確認します。
    /// </summary>
    template<ComponentType T>
    bool Contains() const noexcept
    {
        constexpr TypeInfo info = GetTypeInfo<T>();
        return std::any_of(
            types.begin(), 
            types.end(), 
            [&](const TypeInfo& t) 
            {
                return t.hash == info.hash;
            }
        );
    }
};
