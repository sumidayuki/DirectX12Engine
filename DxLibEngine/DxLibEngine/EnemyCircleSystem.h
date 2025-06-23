#pragma once
#include "TransformSystem.h"

/// <summary>
/// クラス名：EnemyCircleSystem
/// 概要：円運動を行う隊列の生成システムです。
/// 用途：隊列を生成する時、生成数・生成間隔などを確認しながら生成を行います。
/// 備考：Systemを継承しており、Update関数をオーバライドしています。
/// </summary>
class EnemyCircleSystem : public System
{
private:
	TransformSystem* m_transformSystem;

public:
	void Start(ComponentManager& cm, World& world) override;

	void Update(ComponentManager& cm, World& world) override;
};