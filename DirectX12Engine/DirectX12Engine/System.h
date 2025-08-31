#pragma once

/// <summary>
/// クラス名：System
/// 概要：更新処理・描画処理を行う基底クラスです。
/// 用途：このクラスを継承することでECSのシステムとして動作します。
/// 備考：使用するには、worldにSystemを追加する必要があります。
/// </summary>
class System
{
public:
	virtual void Start(ComponentManager& cm, World& world) {};

	/// <summary>
	/// Systemの更新処理を行います。実装は任意です。
	/// </summary>
	/// <param name="cm">コンポーネントマネージャーの参照。Viewを使用する時などに使います。</param>
	/// <param name="world">ワールドの参照。Entityを生成したりなどに使います</param>
	virtual void Update(ComponentManager& cm, World& world) {}

	virtual void InternalRender(ComponentManager& cm, World& world) {}

	/// <summary>
	/// Systemの描画処理を行います。実装は任意です。
	/// </summary>
	/// <param name="cm"></param>
	/// <param name="world"></param>
	virtual void Draw(ComponentManager& cm, World& world) {}

	virtual ~System() = default;
};