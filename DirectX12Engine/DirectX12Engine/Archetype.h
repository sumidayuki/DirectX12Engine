#pragma once
#include "TypeInfo.h"
#include "IComponentData.h"

/// <summary>
/// Archetypeは「コンポーネント集合（Entityの型）」を定義するものです。
/// 含まれるTypeInfoのリストを保持します。
/// SoA化に伴い、オフセット計算は不要になりました。
/// </summary>
struct Archetype
{
    std::vector<TypeInfo> types;             // このArchetypeに含まれる型情報

    /// <summary>
    /// Archetypeを構築します。TypeInfoをソートして一意化します。
    /// </summary>
    Archetype(std::vector<TypeInfo> typeList)
    {
        // ソートして正規化
        std::sort(
            typeList.begin(), 
            typeList.end(), 
            [](auto& a, auto& b) 
            {
                return a.hash < b.hash;
            }
        );

        // 重複削除
        typeList.erase(std::unique(typeList.begin(), typeList.end(),
            [](auto& a, auto& b) { return a.hash == b.hash; }),
            typeList.end());

        types = std::move(typeList);
    }

    /// <summary>
    /// 指定した型のコンポーネントが含まれているか確認します。
    /// </summary>
    bool HasType(const TypeInfo& info) const
    {
        return std::binary_search(
            types.begin(), 
            types.end(), 
            info, 
            [](const TypeInfo& t, const TypeInfo& val)
            {
                return t.hash < val.hash;
            }
        );
    }

    template<typename T>
    bool HasType() const
    {
        return HasType(GetTypeInfo<T>());
    }

    bool operator==(const Archetype& other) const
    {
        if (types.size() != other.types.size()) return false;
        for (size_t i = 0; i < types.size(); ++i)
        {
            if (types[i].hash != other.types[i].hash) return false;
        }
        return true;
    }
};
