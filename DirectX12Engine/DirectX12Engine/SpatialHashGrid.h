#pragma once
#include "Entity.h" // Entityの定義
#include "Vector3.h" // 3D座標を使用

//// 3Dセルの座標構造体
//struct CellCoords3D
//{
//    int x;
//    int y;
//    int z;
//
//    // std::unordered_mapのキーとして使用するために比較演算子を定義
//    bool operator==(const CellCoords3D& other) const
//    {
//        return x == other.x && y == other.y && z == other.z;
//    }
//};
//
//// 3Dセル座標用のカスタムハッシュ関数
//struct CellCoords3DHash
//{
//    size_t operator()(const CellCoords3D& key) const
//    {
//        // シンプルなハッシュ合成 (FNV-1aなどのより良いハッシュ関数があればそれを使う)
//        size_t h1 = std::hash<int>{}(key.x);
//        size_t h2 = std::hash<int>{}(key.y);
//        size_t h3 = std::hash<int>{}(key.z);
//        return h1 ^ (h2 << 1) ^ (h3 << 2);
//    }
//};
//
///// <summary>
///// グリッド上の 1 セルを表します。
///// </summary>
//struct GridCell
//{
//    std::vector<Entity> entities;
//};

/// <summary>
/// 空間上の座標をハッシュ関数でセルに変換し、グリッド構造で管理するクラスです。(3D対応)
/// </summary>
class SpatialHashGrid
{
//private:
//    float m_cellSize; // 修正: m_cellSize に統一
//    // キーを CellCoords3D に、ハッシュ関数を CellCoords3DHash に変更
//    std::unordered_map<CellCoords3D, GridCell, CellCoords3DHash> m_grid;
//
//public:
//    SpatialHashGrid(float size);
//
//    // 戻り値と引数の型を 3D に変更
//    CellCoords3D GetCellCoords(const Vector3& position) const;
//
//    void Clear();
//
//    // 引数の型を 3D に変更
//    void AddEntity(const Entity& entity, const Vector3& position);
//
//    // 戻り値の引数の型を 3D に変更
//    const std::vector<Entity>& GetNearbyEntities(const Vector3& position) const;
};