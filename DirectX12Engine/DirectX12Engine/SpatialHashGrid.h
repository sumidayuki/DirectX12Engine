#pragma once

/// <summary>
/// グリッド上の 1 セルを表します。
/// このセルの中にいる Entity を配列で持ちます。
/// </summary>
struct GridCell 
{
	std::vector<Entity> entities;
};

/// <summary>
/// 空間上の座標をハッシュ関数でセルに変換し、グリッド構造で管理するクラスです。
/// </summary>
class SpatialHashGrid 
{
private:
	float cellSize;
	std::unordered_map<std::pair<int, int>, GridCell, PairHash> grid;

public:
	/// <summary>
	/// 引数付きコンストラクタ
	/// 1セルのサイズを決めます。
	/// </summary>
	/// <param name="size">1セルのサイズ</param>
	SpatialHashGrid(float size) : cellSize(size) {}

	/// <summary>
	/// どのセルにいるのか確認するための関数です。
	/// 例　Vector2(140, 70) なら グリッド座標（2, 1）に変換します。
	/// </summary>
	/// <param name="position">自分の座標</param>
	/// <returns>セルの</returns>
	std::pair<int, int> GetCellCoords(const Vector2& position) const;

	/// <summary>
	/// グリッドの情報全てをクリアします。
	/// これは前フレームの情報をリセットして更新する為です。
	/// </summary>
	void Clear();

	/// <summary>
	/// 指定した座標に対応しているセルに Entity を登録します。
	/// この情報は常に新しい状態にする必要があります。（Entity の座標が変わったりするため）
	/// </summary>
	/// <param name="entity">追加するエンティティ</param>
	/// <param name="position">エンティティの座標</param>
	void AddEntity(const Entity& entity, const Vector2& position);

	/// <summary>
	/// 指定した座標に対応したセルの周囲にある 8セル + 自分のセル（計9セル）を調べて、
	/// そこに登録されている Entity を全て変えします。
	/// </summary>
	/// <param name="position">自分の座標</param>
	/// <returns>std::vector<Entity>型のアドレスを返します。</returns>
	const std::vector<Entity>& GetNearbyEntities(const Vector2& position) const;
};
