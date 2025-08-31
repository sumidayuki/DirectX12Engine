#pragma once

/// <summary>
/// クラス名：Scene
/// 概要：シーンの開始処理・更新処理・描画処理を行う基底クラスです。
/// 用途：このクラスを継承することでシーンをシーンを作成できます。
/// 備考：使用するには、事前にApplicationにシーンを登録しておく必要があります。
/// </summary>
class Scene
{
public:
	bool isStart;

protected:
	World m_world;

	virtual void Start() = 0;	// シーンが始まった時
	virtual void Update() = 0;	// シーンの更新処理
	virtual void Draw() = 0;	// シーンの描画処理

public:
	Scene();

	// デストラクタ
	virtual ~Scene() {}

	void OnStart();

	void OnUpdate();

	void OnDraw();
};